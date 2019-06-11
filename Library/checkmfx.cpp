#include "checkmfx.h"

#include "mfxsession.h"
#include "mfxplugin.h"
#include "mfxvideo.h"

bool CheckMfx::checkMfxSupHw()
{
    mfxIMPL impl   = MFX_IMPL_AUTO_ANY;
    mfxVersion ver = { { MFX_VERSION_MINOR, MFX_VERSION_MAJOR } };
    mfxSession session;

    int ret = MFXInit(impl, &ver, &session);
    if (ret < 0)
    {
       return false;
    }
    MFXQueryIMPL(session, &impl);

    bool isSupHw = false;

    switch (MFX_IMPL_BASETYPE(impl))
    {
    case MFX_IMPL_HARDWARE:
    case MFX_IMPL_HARDWARE2:
    case MFX_IMPL_HARDWARE3:
    case MFX_IMPL_HARDWARE4:
    {
        isSupHw = true;
        break;
    }
    default: break;
    }
    MFXClose(session);
    return isSupHw;
}
