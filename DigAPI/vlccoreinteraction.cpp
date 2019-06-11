#include "vlccoreinteraction.h"
#include "dymediaplayer.h"

#define LIBVLC_USE_PTHREAD_CANCEL 1

#include <vlc/vlc.h>
#include <vlc/plugins/vlc_common.h>
#include <vlc/plugins/vlc_modules.h>
#include <vlc/plugins/vlc_charset.h>
#include <vlc/plugins/vlc_input.h>
#include <vlc/plugins/vlc_messages.h>
#include <vlc/plugins/vlc_playlist.h>
#include <vlc/plugins/vlc_vout.h>
#include <vlc/plugins/vlc_url.h>
#include <vlc/plugins/vlc_interface.h>

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
}

#define qtu(i) ((i).toUtf8().constData())
#define qfu(i) QString::fromUtf8( i )

struct libvlc_event_manager_t
{
    void * p_obj;
    vlc_array_t listeners;
    vlc_mutex_t lock;
};

struct libvlc_media_player_t
{
    VLC_COMMON_MEMBERS

    int                i_refcount;
    vlc_mutex_t        object_lock;

    struct
    {
        input_thread_t   *p_thread;
        input_resource_t *p_resource;
        vlc_renderer_item_t *p_renderer;
        vlc_mutex_t       lock;
    } input;

    struct libvlc_instance_t * p_libvlc_instance; /* Parent instance */
    libvlc_media_t * p_md; /* current media descriptor */
    libvlc_event_manager_t event_manager;
    libvlc_state_t state;
    vlc_viewpoint_t viewpoint;
    int selected_es[3];
};

input_thread_t *libvlc_get_input_thread( libvlc_media_player_t *p_mi );
static vout_thread_t **GetVouts( libvlc_media_player_t *p_mi, size_t *n );
static vout_thread_t *GetVout (libvlc_media_player_t *mp, size_t num);

int asprintf (char **strp, const char *fmt, ...);
int vasprintf (char **strp, const char *fmt, va_list ap);
static void ChangeVFiltersString(const char *psz_name, bool b_add );
static const char* GetVFilterType( const char *psz_name );
static char *ChangeFiltersString( struct vlc_object_t *p_intf, const char *psz_filter_type,
                                  const char *psz_name, bool b_add );
void setFilterOption(const char *psz_module, const char *psz_option, const int &i_int, const double &f_float, const QString &val );

DYMediaPlayer *_player = NULL;

VLCCoreInteraction::VLCCoreInteraction(DYMediaPlayer *player)
{
    _player = player;
}

void VLCCoreInteraction::hflip()
{
    _state = "h";
    flip(1);
}

void VLCCoreInteraction::vflip()
{
    _state = "v";
    flip(2);
}

void VLCCoreInteraction::closeFlip()
{
    _state = "0";
    flip(0);
}

void VLCCoreInteraction::flip(int type)
{
    if(_player->core() == NULL) return;

    const double f_float = -1.0;

    setFilterOption("direct3d9", "d3d-flip-type", type, f_float, "");
}

void VLCCoreInteraction::leftRotate()
{
    _rotateAngle -= 90;
    while(_rotateAngle < 0)
        _rotateAngle += 360;

    this->rotate(_rotateAngle);
}

void VLCCoreInteraction::rightRotate()
{
    _rotateAngle += 90;
    while(_rotateAngle >= 360)
        _rotateAngle -= 360;

    this->rotate(_rotateAngle);
}

void VLCCoreInteraction::rotate(const int &angle)
{
    if(_player->core() == NULL) return;

    _state = QString::number(angle);
    setFilterOption("direct3d9", "d3d-rotate-angle", angle, -1.0, "");
}

void VLCCoreInteraction::closeRotate()
{
    _rotateAngle = 0;
    rotate(0);
}

QString VLCCoreInteraction::toUrl(const QString &s)
{
    if( s.contains( qfu("://") ) )
        return s;

    char *psz = vlc_path2uri( qtu(s), NULL );
    if( psz == NULL )
        return qfu("");

    QString uri = qfu( psz );

    free( psz );
    return uri;
}

#include <QtDebug>
void VLCCoreInteraction::test()
{
    input_item_t* p_item = input_GetItem(libvlc_get_input_thread(_player->core()));

    for( int i = 0; i< p_item->i_categories ; i++) {
        qDebug() << p_item->pp_categories[i]->psz_name;
        for( int j = 0 ; j < p_item->pp_categories[i]->i_infos ; j++ ) {
            qDebug() << p_item->pp_categories[i]->pp_infos[j]->psz_name << " : "
                     << p_item->pp_categories[i]->pp_infos[j]->psz_value;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
input_thread_t *libvlc_get_input_thread( libvlc_media_player_t *p_mi )
{
    input_thread_t *p_input_thread = NULL;

    p_input_thread = p_mi->input.p_thread;
    if( p_input_thread )
        vlc_object_hold( p_input_thread );

    return p_input_thread;
}

static vout_thread_t **GetVouts( libvlc_media_player_t *p_mi, size_t *n )
{
    input_thread_t *p_input = libvlc_get_input_thread( p_mi );
    if( !p_input )
    {
        *n = 0;
        return NULL;
    }
    vout_thread_t **pp_vouts;
    if (input_Control( p_input, INPUT_GET_VOUTS, &pp_vouts, n))
    {
        *n = 0;
        pp_vouts = NULL;
    }
    vlc_object_release (p_input);

    return pp_vouts;
}

static vout_thread_t *GetVout (libvlc_media_player_t *mp, size_t num)
{
    vout_thread_t *p_vout = NULL;
    size_t n;
    vout_thread_t **pp_vouts = GetVouts (mp, &n);
    if (pp_vouts == NULL)
        return p_vout;

    if (num < n)
        p_vout = pp_vouts[num];

    for (size_t i = 0; i < n; ++i)
        if (i != num)
            vlc_object_release ((vlc_object_t *)pp_vouts[i]);

    free (pp_vouts);
    return p_vout;
}


//开启某项功能
static void ChangeVFiltersString(const char *psz_name, bool b_add )
{
    char *psz_string;
    const char *psz_filter_type = GetVFilterType( psz_name );

    vlc_object_t *p_intf = (vlc_object_t *)_player->core();
    psz_string = ChangeFiltersString( p_intf, psz_filter_type, psz_name, b_add );
    if( !psz_string )
        return;

    /* Vout is not kept, so put that in the config */
    config_PutPsz( p_intf, psz_filter_type, psz_string );

    /* Try to set on the fly */
    if( strcmp( psz_filter_type, "video-splitter" ) )
    {
        vout_thread_t *p_vout = GetVout(_player->core(), 0 );
        if( p_vout )
        {
            var_SetString( (vlc_object_t *)p_vout, psz_filter_type, psz_string );
            vlc_object_release( (vlc_object_t *)p_vout );
        }
    }

    free( psz_string );
}

static const char* GetVFilterType(const char *psz_name )
{
    module_t *p_obj = module_find( psz_name );
    if( !p_obj )
    {
        return NULL;
    }

    if( module_provides( p_obj, "video splitter" ) )
        return "video-splitter";
    else if( module_provides( p_obj, "video filter2" ) )
        return "video-filter";
    else if( module_provides( p_obj, "sub source" ) )
        return "sub-source";
    else if( module_provides( p_obj, "sub filter" ) )
        return "sub-filter";
    else if( module_provides( p_obj, "vout display"))
        return "vout";
    else
    {

        //msg_Err((vlc_object_t *)_player->core(), "Unknown video filter type.");
        return NULL;
    }
}

static char *ChangeFiltersString( struct vlc_object_t *p_intf, const char *psz_filter_type,
                                  const char *psz_name, bool b_add )
{
    char *psz_parser, *psz_string;

    psz_string = config_GetPsz( p_intf, psz_filter_type );

    if( !psz_string ) psz_string = strdup( "" );

    psz_parser = strstr( psz_string, psz_name );

    if( b_add )
    {
        if( !psz_parser )
        {
            psz_parser = psz_string;
            if( asprintf( &psz_string, ( *psz_string ) ? "%s:%s" : "%s%s",
                          psz_string, psz_name ) == -1 )
            {
                free( psz_parser );
                return NULL;
            }
            free( psz_parser );
        }
        else
        {
            free( psz_string );
            return NULL;
        }
    }
    else
    {
        if( psz_parser )
        {
            if( *( psz_parser + strlen( psz_name ) ) == ':' )
            {
                memmove( psz_parser, psz_parser + strlen( psz_name ) + 1,
                         strlen( psz_parser + strlen( psz_name ) + 1 ) + 1 );
            }
            else
            {
                *psz_parser = '\0';
            }

            /* Remove trailing : : */
            size_t i_len = strlen( psz_string );
            if( i_len > 0 && *( psz_string + i_len - 1 ) == ':' )
            {
                *( psz_string + i_len - 1 ) = '\0';
            }
        }
        else
        {
            free( psz_string );
            return NULL;
        }
    }
    return psz_string;
}

int asprintf (char **strp, const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start (ap, fmt);
    ret = vasprintf (strp, fmt, ap);
    va_end (ap);
    return ret;
}

int vasprintf (char **strp, const char *fmt, va_list ap)
{
    va_list args;
    int len;

    va_copy (args, ap);
    len = vsnprintf (NULL, 0, fmt, args);
    va_end (args);

    char *str = (char *)malloc (len + 1);
    if (str != NULL)
    {
        int len2;

        va_copy (args, ap);
        len2 = vsprintf (str, fmt, args);
        assert (len2 == len);
        va_end (args);
    }
    else
    {
        len = -1;
#ifndef NDEBUG
        str = (char *)(intptr_t)0x41414141; /* poison */
#endif
    }
    *strp = str;
    return len;
}

void setFilterOption( const char *psz_module, const char *psz_option,
                      const int &i_int, const double &f_float, const QString &val )
{
    vlc_object_t *p_intf = (vlc_object_t *)_player->core();

    vlc_object_t *p_obj = ( vlc_object_t * )vlc_object_find_name( p_intf->obj.libvlc, psz_module );

    int i_type;
    bool b_is_command;

    if( !p_obj )
    {
        i_type = config_GetType( psz_option );
        b_is_command = false;
    }
    else
    {
        i_type = var_Type( p_obj, psz_option );
        if( i_type == 0 )
            i_type = config_GetType( psz_option );
        b_is_command = ( i_type & VLC_VAR_ISCOMMAND );
    }

    i_type &= VLC_VAR_CLASS;
    if( i_type == VLC_VAR_INTEGER || i_type == VLC_VAR_BOOL )
    {
        config_PutInt( p_intf, psz_option, i_int );
        if( b_is_command )
        {
            if( i_type == VLC_VAR_INTEGER )
                var_SetInteger( p_obj, psz_option, i_int );
            else
                var_SetBool( p_obj, psz_option, i_int );
        }
    }
    else if( i_type == VLC_VAR_FLOAT )
    {
        config_PutFloat( p_intf, psz_option, f_float );
        if( b_is_command )
            var_SetFloat( p_obj, psz_option, f_float );
    }
    else if( i_type == VLC_VAR_STRING )
    {
        config_PutPsz( p_intf, psz_option, qtu( val ) );
        if( b_is_command )
            var_SetString( p_obj, psz_option, qtu( val ) );
    }

    if( !b_is_command )
    {
        ChangeVFiltersString( psz_module, false );
        ChangeVFiltersString( psz_module, true );
    }

    if( p_obj ) vlc_object_release( p_obj );
}
