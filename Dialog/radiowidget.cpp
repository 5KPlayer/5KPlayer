#include "radiowidget.h"
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QDebug>
#include <QFileInfo>
#include "dylabel.h"
#include "openfile.h"
#include "dymediaplayer.h"
#include "liverecentplay.h"
#include "liveicomodewidget.h"
#include "liveurl.h"
#include "liveico.h"
#include "globalarg.h"
#include "sqloperation.h"
#include "playermainwidget.h"

RadioWidget::RadioWidget(QWidget *parent) : UpdateInterfaceWidget(parent)
{
    _playBtn = new DYLabel(this,true);
    _playBtn->setText(Lge->getLangageValue("Main/play"));
    _playBtn->setAlignment(Qt::AlignCenter);
    _playBtn->resize(76*Global->_screenRatio, 20*Global->_screenRatio);
    _recentWidget  = new LiveRecentPlay(this);
    _liveIcoWidget = new LiveIcoModeWidget(this);

    _liveIcoWidget->setStyleSheet(".LiveIcoModeWidget{border-style: solid; border-width: 2px; border-color: rgb(46,46,51); border-radius: 10px;}");
    QPalette p1;
    p1.setColor(QPalette::WindowText,Qt::white);

    _liveUrlList << "http://bbcmedia.ic.llnwd.net/stream/bbcmedia_radio5live_mf_p?s=1428556760&e=1428571160&h=ca7f382f56fdebc5f3ea3c23e5e62a3a"
                 << "http://1661.live.streamtheworld.com:80/WJFKAMAAC_SC?TGT=iTunes&DIST=CBS&SRC=CBS"
                 << "http://stream.us.gslb.liquidcompass.net/KDUSAMAAC?ats=1"
                 << "http://streamingads.hotmixradio.fm/hotmixradio-80-64.aac"
                 << "http://streaming.radionomy.com/ABC-Lounge"
                 << "http://ice.stream.frequence3.net/frequence3-128.mp3"
                 << "http://streaming.radionomy.com/atomik_radio"
                 << "http://online-kissfm.tavrmedia.ua/KissFM"
                 << "http://streaming.radionomy.com/The-Buffalo"
                 << "http://streaming206.radionomy.com:8888/The-Music-Radio"
                 << "http://streaming.radionomy.com/BestRadio"
                 << "http://streaming.radionomy.com/heart-fm";
    _liveImaList << ":/res/RadioPreset/bbc.jpg"         << ":/res/RadioPreset/cbssports.jpg"
                 << ":/res/RadioPreset/nbcsports.png"   << ":/res/RadioPreset/hotmix80.png"
                 << ":/res/RadioPreset/abclounge.jpeg"  << ":/res/RadioPreset/frequence.jpg"
                 << ":/res/RadioPreset/atomikradio.jpg" << ":/res/RadioPreset/kissfm.png"
                 << ":/res/RadioPreset/buffalo.jpg"     << ":/res/RadioPreset/musicradio.jpg"
                 << ":/res/RadioPreset/bestradio.jpg"   << ":/res/RadioPreset/heartfm.jpg";

    Sql->initLiveUrl(&_favliveList,2);
    _space = 20 * Global->_screenRatio;

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * Global->_screenRatio);

    _radioLabel = new QLabel(Lge->getLangageValue("Live/input"), this);
    _radioLabel->setFont(font);
    _radioLabel->setPalette(p1);
    _radioLabel->adjustSize();

    _radioAddr = new QLineEdit(this);
    _radioAddr->setFont(font);
    _radioAddr->setContextMenuPolicy(Qt::NoContextMenu);
    _radioAddr->setStyleSheet(Global->lineEditStyleSheet(4));

    font.setBold(true);
    _playBtn->setFont(font);
    connect(_playBtn, SIGNAL(clicked()), SLOT(playRadio()));
    connect(_recentWidget,SIGNAL(upWidgetUI()),SLOT(slotupDataUI()));
    connect(_recentWidget,SIGNAL(sendLivePlay(QString)),SLOT(playLiveUrl(QString)));
    connect(_liveIcoWidget,SIGNAL(upDataLiveFav(QString,bool)),SLOT(upLiveFavirtes(QString,bool)));
    connect(_liveIcoWidget,SIGNAL(livePlay(QString)),SLOT(playLiveUrl(QString)));
    connect(_liveIcoWidget,SIGNAL(liveShow(QString)),SLOT(showLiveUrl(QString)));

    upRecentWidget();
}

void RadioWidget::updateUI(const qreal &scaleRatio)
{
    _recentWidget->updateUI(scaleRatio);
    _liveIcoWidget->updateUI(scaleRatio);
    _space = 20 * scaleRatio;

    QFont font;
    font.setFamily(Global->getFontFamily());
    font.setPixelSize(12 * scaleRatio);

    _playBtn->setFont(font);
    _radioLabel->setFont(font);
    _radioLabel->adjustSize();
    _radioAddr->setFont(font);
    _radioAddr->setStyleSheet(Global->lineEditStyleSheet(4 * scaleRatio));

    _playBtn->resize(76 * scaleRatio, 20 * scaleRatio);
    _radioAddr->resize(this->width() - _playBtn->width() - _space *4, 20 * scaleRatio);
}

void RadioWidget::upRecentWidget()
{
    _liveIcoWidget->clearLiveWidget();
    for(int i=0;i<_favliveList.size();++i) {
        for(int j = 0;j<_liveUrlList.size();++j) {
            if(_favliveList.at(i) == _liveUrlList.at(j)) {
                _liveIcoWidget->addLiveWidget(_liveUrlList.at(j),_liveImaList.at(j),true);
                break;
            }
        }
    }

    for(int i = 0;i<_liveUrlList.size();++i) {
        bool isfav = false;
        for(int j=0;j<_favliveList.size();++j) {
            if(_liveUrlList.at(i) == _favliveList.at(j)){
                isfav = true;
                break;
            }
        }
        if(!isfav)
            _liveIcoWidget->addLiveWidget(_liveUrlList.at(i),_liveImaList.at(i),false);
    }
    _liveIcoWidget->updateLiveWidget();
}

void RadioWidget::resizeEvent(QResizeEvent *)
{
    double _LivePathWidth = this->width() - _playBtn->width() - _space *3;
    if(_LivePathWidth > 700*Global->_screenRatio) {
        double xPos = (this->width() - 700*Global->_screenRatio - _playBtn->width() - _space)/2;
        _radioLabel->move(xPos, _space);
        _radioAddr->setGeometry(xPos, _radioLabel->y() + _radioLabel->height() + _space/2,
                                700*Global->_screenRatio,
                                20 * Global->_screenRatio);

        _recentWidget->setGeometry(xPos,_radioAddr->y()+_radioAddr->height()+_space,
                                   _radioAddr->width()+ _space + _playBtn->width(),
                                   _recentWidget->height());

        _liveIcoWidget->setGeometry(xPos,
                                    _recentWidget->y()+_recentWidget->height()+_space,
                                    _radioAddr->width()+_space+_playBtn->width(),
                                    this->height()-(_recentWidget->y()+_recentWidget->height()+_space*3));
    } else {
        _radioLabel->move(_space, _space);
        _radioAddr->setGeometry(_space, _radioLabel->y() + _radioLabel->height() + _space/2,
                                this->width() - _playBtn->width() - _space*3,
                                20 * Global->_screenRatio);

        _recentWidget->setGeometry(_space,_radioAddr->y()+_radioAddr->height()+_space,
                                   _radioAddr->width()+ _space + _playBtn->width(),
                                   _recentWidget->height());

        _liveIcoWidget->setGeometry(_space,_recentWidget->y()+_recentWidget->height()+_space,
                                    _radioAddr->width()+_space+_playBtn->width(),
                                    this->height()-(_recentWidget->y()+_recentWidget->height()+_space*3));
    }
    _playBtn->move(_radioAddr->x() + _radioAddr->width() + _space, _radioAddr->y());
}

void RadioWidget::showClickedRadioAddr(const QString &addr)
{
    _radioAddr->setText(addr);
    _radioAddr->setCursorPosition(0);
}

void RadioWidget::playRadio()
{
    const QString radioStr = _radioAddr->text().trimmed();
    if(radioStr.isEmpty())
        return;

    // 是否是有图片的Live链接和是否是链接，不是才加入历史播放记录
    if (radioStr.startsWith("http://") || radioStr.startsWith("https://")) {
        bool isliveUrl = false;
        int imagePos = -1;
        for(int i=0;i<_liveUrlList.size();++i) {
            if(_liveUrlList.at(i).toLower() == radioStr.toLower()) {
                imagePos = i;
                isliveUrl = true;
                break;
            }
        }
        if(!isliveUrl) {
            _recentWidget->addRecent(radioStr);
            Global->setLiveInfo("","");
        } else {
            QFileInfo info(_liveImaList.at(imagePos));
            Global->setLiveInfo(_liveImaList.at(imagePos),info.baseName());
        }
    }


    OpenFileDialog->setPlayType(OPenFile::Live);
    MediaPlayer->playFile(radioStr, false);

    //单击播放 关闭当前窗口
    qobject_cast<QDialog *>(this->parent())->accept();
}

void RadioWidget::playLiveUrl(const QString &liveUrl)
{
    const QString radioStr = liveUrl;
    if(radioStr.isEmpty())
        return;

    bool isliveUrl = false;
    int imagePos = -1;
    for(int i=0;i<_liveUrlList.size();++i) {
        if(_liveUrlList.at(i).toLower() == radioStr.toLower()) {
            imagePos = i;
            isliveUrl = true;
            break;
        }
    }
    if(!isliveUrl) {
        _recentWidget->addRecent(radioStr);
        Global->setLiveInfo("","");
    } else {
        QFileInfo info(_liveImaList.at(imagePos));
        Global->setLiveInfo(_liveImaList.at(imagePos),info.baseName());
    }

    OpenFileDialog->setPlayType(OPenFile::Live);
    MediaPlayer->playFile(radioStr, false);

    //单击播放 关闭当前窗口
    qobject_cast<QDialog *>(this->parent())->accept();
}

void RadioWidget::slotupDataUI()
{
    //_recentWidget->move(_radioAddr->x(),_radioAddr->y()+_radioAddr->height()+_space);
    _liveIcoWidget->setGeometry(_radioAddr->x(), _recentWidget->y() + _recentWidget->height() + _space,
                                _radioAddr->width()+_space+_playBtn->width(),
                                this->height()-(_recentWidget->y()+_recentWidget->height()+_space*3));
}

void RadioWidget::upLiveFavirtes(const QString &liveUrl, bool fav)
{
    if(fav) {
        _favliveList.prepend(liveUrl);
        Sql->addLiveurlToSql(liveUrl,2);
    } else {
        for(int i=0;i<_favliveList.size();++i) {
            if(_favliveList.at(i) == liveUrl) {
                _favliveList.removeAt(i);
                break;
            }
        }
        Sql->removeLiveUrl(liveUrl);
    }
    upRecentWidget();
}

void RadioWidget::showLiveUrl(const QString &url)
{
    _radioAddr->setText(url);
}
