#include "setleftnode.h"
#include "globalarg.h"
SetLeftNode::SetLeftNode(QWidget *parent)
    : TransEventWidget(parent)
{
    _isSelect   = false;
    Init();
}

void SetLeftNode::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    upUI();
}

void SetLeftNode::setNodeData(QString icoPath, QString title)
{
    _svgPath    = icoPath;
    _nameText   = title;
    QString loadPath;
    if(_isSelect) {
        loadPath = QString(":/res/svg/%1_%2.svg").arg(_svgPath).arg("on");
    } else {
        loadPath = QString(":/res/svg/%1_%2.svg").arg(_svgPath).arg("nor");
    }
    _ico->load(loadPath);
    _name->setText(title);
}

void SetLeftNode::setFontColor(QString color)
{
    _name->setStyleSheet(QString("color:%1").arg(color));//("color: rgb(255,255,255);");
}

void SetLeftNode::setSelect(bool select)
{
    _isSelect = select;
    QString loadPath;
    if(_isSelect) {
        loadPath = QString(":/res/svg/%1_%2.svg").arg(_svgPath).arg("on");
        this->setStyleSheet("background-color: rgb(32, 32, 33);");
        setFontColor("rgb(255,255,255)");
    } else {
        loadPath = QString(":/res/svg/%1_%2.svg").arg(_svgPath).arg("nor");
        this->setStyleSheet("background-color: rgb(14, 14, 15);");
        setFontColor("rgb(92,94,102)");
    }
    _ico->load(loadPath);
}

void SetLeftNode::Init()
{
    _ico   = new TransEventIco(this);
    _name  = new QLabel(this);

    _name->setStyleSheet("color: rgb(92,94,102);");
    _scaleRatio = 1.0;
    upUI();
}

void SetLeftNode::upUI()
{
    int labelW  = this->width() - 36 *_scaleRatio;
    _ico->setGeometry(8*_scaleRatio,(this->height()-17*_scaleRatio)/2,20*_scaleRatio,17*_scaleRatio);
    _name->setGeometry(_ico->x()+_ico->width()+8*_scaleRatio,(this->height()-20*_scaleRatio)/2,labelW,20*_scaleRatio);
    QFont font;
    font.setPixelSize(13*_scaleRatio);
    font.setFamily(Global->getFontFamily());
    font.setWeight(57);
    font.setLetterSpacing(QFont::AbsoluteSpacing,1);
    _name->setFont(font);

    int nameW   = _name->fontMetrics().width(_nameText);
    if(nameW > labelW) {
        for(int i=_nameText.size()-1; i >= 0;i--) {
            QString showText = _nameText.left(i).append("...");
            int showW = _name->fontMetrics().width(showText);
            if(showW < labelW) {
                _name->setText(showText);
                break;
            }
        }
    } else {
        _name->setText(_nameText);
    }
}

void SetLeftNode::resizeEvent(QResizeEvent *)
{
    upUI();
}

void SetLeftNode::enterEvent(QEvent *)
{
    if(!_isSelect)
        this->setStyleSheet("background-color: rgb(51, 51, 56);");
}

void SetLeftNode::leaveEvent(QEvent *)
{
    if(!_isSelect)
        this->setStyleSheet("background-color: rgb(14, 14, 15);");
}
