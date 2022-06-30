﻿#include "enemy.h"
#include "mainwindow.h"
#include "tower.h"
#include "utility.h"
#include "waypoint.h"

#include <QPainter>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QVector2D>

const QSize Enemy::m_size(55,65);

Enemy::Enemy(wayPoint* startWayPoint,int Hp,int walkingSpeed,MainWindow* game,QString path):
    QObject(0),m_maxHp(Hp),m_walkingSpeed(walkingSpeed),m_game(game),m_pos(startWayPoint->getPos()),m_path(path){
    m_currentHp=m_maxHp;
    m_active=false;
    m_destinationWayPoint=startWayPoint->getNextWayPoint();
}
Enemy::~Enemy(){
    m_destinationWayPoint=NULL;
    m_game=NULL;
}
///绘出enemy

void Enemy::draw(QPainter* painter)const{
    if(!m_active) return;

    painter->save();
    ///画出敌人的血条
    static const int healthBarWidth=m_size.width();//血条长度
    QPoint healthBarPoint=m_pos+QPoint(-m_size.width()/2,-m_size.height());
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    QRect healthBarBackRect(healthBarPoint,QSize(healthBarWidth,2));
    painter->drawRect(healthBarBackRect);
    ///画出当前血量
    painter->setBrush(Qt::green);
    ///算出当前血量占比
    QRect healthBarRect(healthBarPoint,QSize((double)m_currentHp/m_maxHp*healthBarWidth,2));
    painter->drawRect(healthBarRect);
    ///画敌人
    QPoint tmp(m_pos.x()-m_size.width()/2,m_pos.y()-m_size.height()/2);//得到图片左上点
    painter->drawPixmap(tmp.x(),tmp.y(),m_path);
    painter->restore();
}

void Enemy::move(){
    if(!m_active)return;
    if(isIncide(m_pos,1,m_destinationWayPoint->getPos(),1)){//到达目标航点
        if(m_destinationWayPoint->getNextWayPoint()){
            m_pos=m_destinationWayPoint->getPos();
            m_destinationWayPoint=m_destinationWayPoint->getNextWayPoint();

        }
        else{
            m_game->getHpDamaged();
            m_game->removeEnemy(this);
            return;
        }
    }
    else{
        QPoint targetPoint  = m_destinationWayPoint->getPos();
        double movementSpeed=m_walkingSpeed;
        QVector2D normailzed(targetPoint-m_pos);
        normailzed.normalize();
        m_pos=m_pos+normailzed.toPoint()*movementSpeed;
    }
}

void Enemy::doActive(){
    m_active=true;
}

QPoint Enemy::getPos(){
    return m_pos;
}

void Enemy::getDamaged(int damage)
{
    m_currentHp-=damage;
    if(m_currentHp<=0)
    {
        m_game->awardGold();
        getRemoved();
    }
}
void Enemy::getRemoved(){
    m_game->removeEnemy(this);
}

