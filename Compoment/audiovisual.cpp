#include "audiovisual.h"
#include <QPainter>
#include <QPaintEvent>
#include <QtDebug>

AudioVisual::AudioVisual(uint count, QWidget *parent) : QWidget(parent)
{
    m_i_value  = 0;
    m_i_width  = 3;
    m_i_height = 20;
    m_i_margin = 4;
    m_i_count  = count;
    _scaleRatio = 1.0;
}

void AudioVisual::updateUI(const qreal &scaleRatio)
{
    _scaleRatio = scaleRatio;
    m_i_width  = 3*_scaleRatio;
    m_i_height = 20*_scaleRatio;
    m_i_margin = 4*_scaleRatio;
}

void AudioVisual::setCurValue(uint value)
{
    if(value <= m_i_count && value != m_i_value)
    {
        m_i_value = value;
        //update();
        repaint();
    }
}

QSize AudioVisual::sizeHint() const
{
    return QSize(2*(m_i_count*(m_i_width+m_i_margin)+m_i_margin)+m_i_margin, m_i_height+m_i_margin*2);
}

void AudioVisual::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    //p.drawRect(1,1,this->width()-2,this->height()-2);

    for(uint i=0;i<m_i_count;++i)
    {
        if(i<m_i_value)
            p.fillRect(QRect((m_i_count-1-i)*(m_i_width+m_i_margin)+m_i_margin, m_i_margin, m_i_width, m_i_height) ,
                       Qt::green);
        else
            p.fillRect(QRect((m_i_count-1-i)*(m_i_width+m_i_margin)+m_i_margin, m_i_margin, m_i_width, m_i_height) ,
                       Qt::gray);
    }

    const int x = m_i_count*(m_i_width+m_i_margin)+m_i_margin*2;

    for(uint i=0;i<m_i_count;++i)
    {
        if(i<m_i_value)
            p.fillRect(QRect(x+i*(m_i_width+m_i_margin)+m_i_margin, m_i_margin, m_i_width, m_i_height) ,
                        Qt::green);
        else
            p.fillRect(QRect(x+i*(m_i_width+m_i_margin)+m_i_margin, m_i_margin, m_i_width, m_i_height) ,
                       Qt::gray);
    }

}
