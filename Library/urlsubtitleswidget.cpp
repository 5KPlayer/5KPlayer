#include "urlsubtitleswidget.h"
#include <QLocale>
#include "globalarg.h"
#include <QDebug>
URLSubtitlesWidget::URLSubtitlesWidget(QWidget *parent)
    : TransEventWidget(parent)
{
    Init();
}

void URLSubtitlesWidget::updateLanguage()
{
    for(int i=0;i<10;i++) {
        _checkBox[i]->updateLanguage();
    }
}

void URLSubtitlesWidget::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    for(int i=0;i<10;++i) {
        _checkBox[i]->updateUI(scaleRatio);
    }
    upUI();
}

void URLSubtitlesWidget::setSubTitle(const QStringList &subData, const QStringList &checkList)
{
    _subList = subData;
    _checkSubtitle = checkList;
    _devValue = 0;
    upSubWidget();
}

QStringList URLSubtitlesWidget::getSubTitle()
{
    return _subList;
}

void URLSubtitlesWidget::clearSubTitle()
{
    _subList.clear();
    _checkSubtitle.clear();
    _devValue = 0;
    upSubWidget();
}

QStringList URLSubtitlesWidget::getCheckSubTitle()
{
    return _checkSubtitle;
}

void URLSubtitlesWidget::checkBoxClick(bool check)
{
    int checkPos = 0;
    for(int i=_devValue;i<_subList.size();++i) {
        if(check) {
            for(int j=0;j<_showCount;++j) {
                if(_checkBox[j]->isChecked()) {
                    addtoSubtitle(_devValue+j);
                }
            }
        } else {
            for(int j=0;j<_showCount;++j) {
                if(!_checkBox[j]->isChecked()) {
                    removetoSubtitle(_devValue+j);
                }
            }
        }
        if(checkPos > 9)
            break;
        checkPos++;
    }
}

void URLSubtitlesWidget::leftBtnClick()
{
    // 这里算法需要改进，需要检查上10个最多能放多少个在字幕面板，
    if(_devValue > 0) {
        if(_devValue <= 10) {
            _devValue -= _devValue;
        } else {
            _devValue -= 10;
        }
    }
    upSubWidget();
}

void URLSubtitlesWidget::rightBtnClick()
{
    if(0<(_subList.size()-_curShowCount-_devValue)) {
        _devValue+=_curShowCount;
        upSubWidget();
    }
}


void URLSubtitlesWidget::Init()
{
    _devValue       = 0;
    _scaleRatio     = 1.0;

    _leftBtn = new DYLabel(this,true);
    _rightBtn= new DYLabel(this,true);

    _leftBtn->hide();
    _rightBtn->hide();
    _leftBtn->setText("<");
    _rightBtn->setText(">");
    _leftBtn->setAlignment(Qt::AlignCenter);
    _rightBtn->setAlignment(Qt::AlignCenter);
    for(int i=0;i<10;++i) {
        _checkBox[i] = new DYCheckBox(this);
        connect(_checkBox[i],SIGNAL(checkChange(bool)),SLOT(checkBoxClick(bool)));
        _checkBox[i]->setStyleSheet("Color:rgb(182,183,184)");
        _checkBox[i]->hide();
    }
    connect(_leftBtn,SIGNAL(clicked()),SLOT(leftBtnClick()));
    connect(_rightBtn,SIGNAL(clicked()),SLOT(rightBtnClick()));

}

void URLSubtitlesWidget::upSubWidget()
{
    _showCount = 0;
    hideAllCheckBox();
    for(int i=_devValue;i<_subList.size();++i) {
        bool checkFlag = false;
        foreach(QString subLanguage, _checkSubtitle) {
            if(_subList.at(i) == subLanguage) {
                checkFlag = true;
                break;
            }
        }

        _checkBox[_showCount]->show();
        _checkBox[_showCount]->setChecked(checkFlag);
        _checkBox[_showCount]->setText(languagetoString(_subList.at(i)));
        _showCount++;
        if(_showCount>9)
            break;
    }
    upUI();
}

void URLSubtitlesWidget::upUI()
{
    _curShowCount   = 0;
    int  spacing    = 5*_scaleRatio;
    int  YPos0      = 5*_scaleRatio;
    int  YPos1      = 30*_scaleRatio;
    int  height     = 15*_scaleRatio;
    int  btnHeight  = 15*_scaleRatio;
    int  maxWidth= 0;
    int  length = 0;

    if(_subList.size() == 0) {
        _leftBtn->hide();
        _rightBtn->hide();
        return;
    } else {
        _leftBtn->show();
        _rightBtn->show();
    }
    if(_subList.size() == 1)
        YPos0 = (this->height()-height)/2;
    int  checkWidgetW = this->width() - (spacing+btnHeight);

    _leftBtn->setGeometry(spacing,(this->height()-btnHeight)/2,btnHeight,btnHeight);
    _rightBtn->setGeometry(this->width()-spacing-btnHeight,(this->height()-btnHeight)/2,btnHeight,btnHeight);

    length = spacing+btnHeight;
    int width0 = getCheckWidth(_checkBox[0]->text());
    _checkBox[0]->setGeometry(spacing+length,YPos0,width0,height);
    if(_showCount>0) {
        _checkBox[0]->show();
        _curShowCount++;
    } else
        _checkBox[0]->hide();
    int width1 = getCheckWidth(_checkBox[1]->text());
    if(_showCount>1) {
        _checkBox[1]->show();
        _curShowCount++;
    } else
        _checkBox[1]->hide();
    _checkBox[1]->setGeometry(spacing+length,25*_scaleRatio,width1,height);
    maxWidth = width0>width1?width0:width1;
    length += spacing + maxWidth;

    width0 = getCheckWidth(_checkBox[2]->text());
    width1 = getCheckWidth(_checkBox[3]->text());
    maxWidth = width0>width1?width0:width1;

    if(checkWidgetW > length + spacing + maxWidth) {
        _checkBox[2]->setGeometry(spacing+length,YPos0,maxWidth,height);
        _checkBox[3]->setGeometry(spacing+length,YPos1,maxWidth,height);
        if(_showCount>2) {
            _checkBox[2]->show();
            _curShowCount++;
        } else
            _checkBox[2]->hide();
        if(_showCount>3) {
            _checkBox[3]->show();
            _curShowCount++;
        } else
            _checkBox[3]->hide();
    } else {
        _checkBox[2]->hide();
        _checkBox[3]->hide();
    }
    length += spacing + maxWidth;

    width0 = getCheckWidth(_checkBox[4]->text());
    width1 = getCheckWidth(_checkBox[5]->text());
    maxWidth = width0>width1?width0:width1;
    if(checkWidgetW > length + spacing + maxWidth) {
        _checkBox[4]->setGeometry(spacing+length,YPos0,maxWidth,height);
        _checkBox[5]->setGeometry(spacing+length,YPos1,maxWidth,height);
        if(_showCount>4) {
            _checkBox[4]->show();
            _curShowCount++;
        } else
            _checkBox[4]->hide();
        if(_showCount>5) {
            _checkBox[5]->show();
            _curShowCount++;
        } else
            _checkBox[5]->hide();
    } else {
        _checkBox[4]->hide();
        _checkBox[5]->hide();
    }
    length += spacing + maxWidth;

    width0 = getCheckWidth(_checkBox[6]->text());
    width1 = getCheckWidth(_checkBox[7]->text());
    maxWidth = width0>width1?width0:width1;

    if(checkWidgetW > length + spacing + maxWidth) {
        _checkBox[6]->setGeometry(spacing+length,YPos0,maxWidth,height);
        _checkBox[7]->setGeometry(spacing+length,YPos1,maxWidth,height);
        if(_showCount>6) {
            _checkBox[6]->show();
            _curShowCount++;
        } else
            _checkBox[6]->hide();
        if(_showCount>7) {
            _checkBox[7]->show();
            _curShowCount++;
        } else
            _checkBox[7]->hide();
    } else {
        _checkBox[6]->hide();
        _checkBox[7]->hide();
    }
    length += spacing + maxWidth;

    width0 = getCheckWidth(_checkBox[8]->text());
    width1 = getCheckWidth(_checkBox[9]->text());
    maxWidth = width0>width1?width0:width1;

    if(checkWidgetW > length + spacing + maxWidth) {
        _checkBox[8]->setGeometry(spacing+length,YPos0,maxWidth,height);
        _checkBox[9]->setGeometry(spacing+length,YPos1,maxWidth,height);
        if(_showCount>8) {
            _checkBox[8]->show();
            _curShowCount++;
        } else
            _checkBox[8]->hide();
        if(_showCount>9) {
            _checkBox[9]->show();
            _curShowCount++;
        } else
            _checkBox[9]->hide();
    } else {
        _checkBox[8]->hide();
        _checkBox[9]->hide();
    }
}

int URLSubtitlesWidget::getCheckWidth(const QString &checkText)
{
    int width = _checkBox[0]->fontMetrics().width(checkText)+42*_scaleRatio;
    return width;
}

void URLSubtitlesWidget::hideAllCheckBox()
{
    for(int i=0;i<10;++i) {
        _checkBox[i]->hide();
    }
}

void URLSubtitlesWidget::addtoSubtitle(int pos)
{
    bool addFlag = true;
    // 检查是否已经在列表中，在则不添加
    foreach (QString subStr, _checkSubtitle) {
        if(subStr == _subList.at(pos)) {
            addFlag = false;
            break;
        }
    }
    if(addFlag)
        _checkSubtitle.append(_subList.at(pos));

}

void URLSubtitlesWidget::removetoSubtitle(int pos)
{
    bool delFlag = false;
    int  delPos = 0;
    // 检查是否已经在列表中，不在则不删除
    for(int i=0;i<_checkSubtitle.size();++i) {
        if(_checkSubtitle.at(i) == _subList.at(pos)) {
            delFlag = true;
            delPos = i;
            break;
        }
    }

    if(delFlag) {
        _checkSubtitle.removeAt(delPos);
    }
}

QString URLSubtitlesWidget::languagetoString(const QString &language)
{
    QLocale korean(language);
    return korean.nativeLanguageName();
}

void URLSubtitlesWidget::resizeEvent(QResizeEvent *)
{
    upUI();
}
