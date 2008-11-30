//  $Id$
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2004-2005 Steve Baker <sjbaker1@airmail.net>
//  Copyright (C) 2006 SuperTuxKart-Team, Joerg Henrichs, Steve Baker
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_KART_H
#define HEADER_KART_H

#define _WINSOCKAPI_
#include <plib/sg.h>
#include "btBulletDynamicsCommon.h"

#include "terrain_info.hpp"
#include "karts/moveable.hpp"
#include "karts/kart_properties.hpp"
#include "karts/kart_control.hpp"
#include "items/attachment.hpp"
#include "items/powerup.hpp"
#include "karts/kart_model.hpp"

class SkidMark;
class Item;
class Smoke;
class SFXBase;

class Kart : public TerrainInfo, public Moveable
{
private:
    btTransform  m_reset_transform;    // reset position
    unsigned int m_world_kart_id;      // index of kart in world
    float        m_skidding;           /**< Accumulated skidding factor. */

protected:
    Attachment   m_attachment;
    Powerup      m_powerup;
    int          m_race_position;      // current race position (1-numKarts)
    int          m_initial_position;   // initial position of kart

    KartControl  m_controls;           // The position of the karts controls

    float        m_max_speed;          // maximum speed of the kart, computed from
    float        m_max_gear_rpm;       //maximum engine rpm's for the current gear
    float        m_max_speed_reverse_ratio;
    float        m_wheelie_angle;
    float        m_zipper_time_left;   // zipper time left
    float        m_bounce_back_time;   // a short time after a collision acceleration
                                       // is disabled to allow the karts to bounce back

    // physics parameters, storing it saves time
    btKart::btVehicleTuning           *m_tuning;
    btCompoundShape                    m_kart_chassis;
    btVehicleRaycaster                *m_vehicle_raycaster;
    btKart                            *m_vehicle;
    btUprightConstraint               *m_uprightConstraint;

private:
    /** The amount of energy collected bu hitting coins. */
    float               m_collected_energy;
    ssgSimpleState*     m_smokepuff;
    // don't delete the following 2 vars (they're kids in the hirarchy)
    Smoke              *m_smoke_system;
    ssgTransform*       m_exhaust_pipe;

    float               m_wheel_rotation;
    /** For each wheel it stores the suspension length after the karts are at 
     *  the start position, i.e. the suspension will be somewhat compressed.
     *  The bullet suspensionRestLength is the value when the suspension is not
     *  at all compressed. */
    float               m_default_suspension_length[4];

    SkidMark*           m_skidmark_left;
    SkidMark*           m_skidmark_right;


    float               m_finish_time;
    bool                m_finished_race;

    float               m_speed;
    float               m_rpm;
    float               m_current_gear_ratio;
    bool                m_rescue;
    bool                m_eliminated;

    SFXBase            *m_engine_sound;
    SFXBase            *m_beep_sound;
    SFXBase            *m_crash_sound;
    SFXBase            *m_skid_sound;

protected:
    float                 m_rescue_pitch, m_rescue_roll;
    const KartProperties *m_kart_properties;

    
public:
                   Kart(const std::string& kart_name, int position, 
                        const btTransform& init_transform);
    virtual       ~Kart();
    unsigned int   getWorldKartId() const            { return m_world_kart_id;   }
    void           setWorldKartId(unsigned int n)    { m_world_kart_id=n;        }
    void           loadData();
    virtual void   updateGraphics      (const Vec3& off_xyz,  const Vec3& off_hpr);
    const KartProperties* 
                   getKartProperties   () const      { return m_kart_properties; }
    void           setKartProperties   (const KartProperties *kp)
                                          { m_kart_properties=kp;                }
    void           attach              (attachmentType attachment_, float time)
                                          { m_attachment.set(attachment_, time); }
    void           setPowerup      (PowerupType t, int n)
                                          { m_powerup.set(t, n);             }
    virtual void   setPosition         (int p)    
                                          { m_race_position = p;                 }
    
    Attachment    *getAttachment       ()       { return &m_attachment;          }
    void           setAttachmentType   (attachmentType t, float time_left=0.0f,
                                        Kart*k=NULL)
                                          { m_attachment.set(t, time_left, k);   }
    Powerup       *getPowerup          ()       { return &m_powerup;         }
    int            getNumPowerup       () const { return  m_powerup.getNum();}
    float          getEnergy           () const { return  m_collected_energy;}
    int            getPosition         () const { return  m_race_position;       }
    int            getInitialPosition  () const { return  m_initial_position;    }
    float          getFinishTime       () const { return  m_finish_time;         }
    bool           hasFinishedRace     () const { return  m_finished_race;       }
    void           endRescue           ();
    //float          estimateFinishTime  ();
    void           processSkidMarks    ();
    void           getClosestKart      (float *cdist, int *closest);
    void           updatePhysics       (float dt);

   /**
       returns a bullet transform object located at the kart's position
       and oriented in the direction the kart is going. Can be useful
       e.g. to calculate the starting point and direction of projectiles
    */
   btTransform    getKartHeading      (const float customPitch=-1);

    
    // Functions to access the current kart properties (which might get changed,
    // e.g. mass increase or air_friction increase depending on attachment etc.)
    // -------------------------------------------------------------------------
    const Vec3    &getColor         () const {return m_kart_properties->getColor();}
    float          getMass          () const
    {
        return m_kart_properties->getMass()
               + m_attachment.weightAdjust();
    }
    float          getMaxPower      () const {return m_kart_properties->getMaxPower();}
    float          getTimeFullSteer () const {return m_kart_properties->getTimeFullSteer();}
    float          getBrakeFactor   () const {return m_kart_properties->getBrakeFactor();}
    float          getFrictionSlip  () const {return m_kart_properties->getFrictionSlip();}
    float          getMaxSteerAngle () const
                       {return m_kart_properties->getMaxSteerAngle(getSpeed());}
    const Vec3&    getGravityCenterShift   () const
        {return m_kart_properties->getGravityCenterShift();                    }
    float          getWheelieMaxSpeedRatio () const
        {return m_kart_properties->getWheelieMaxSpeedRatio();                  }
    float          getWheelieMaxPitch  () const
        {return m_kart_properties->getWheelieMaxPitch();                       }
    float          getWheeliePitchRate () const
        {return m_kart_properties->getWheeliePitchRate();                      }
    float          getWheelieRestoreRate() const
        {return m_kart_properties->getWheelieRestoreRate();                    }
    float          getWheelieSpeedBoost() const
        {return m_kart_properties->getWheelieSpeedBoost();                     }
    float          getSteerPercent  () const {return m_controls.lr;            }
    const KartControl&
                   getControls      () const {return m_controls;               }
    /** Sets the kart controls. Used e.g. by replaying history. */
    void           setControls(const KartControl &c) { m_controls = c;         }
    float          getMaxSpeed      () const {return m_max_speed;              }
    /** Returns the length of the kart. */
    float          getKartLength    () const
                   {return m_kart_properties->getKartModel()->getLength();     }
    /** Returns the height of the kart. */
    float          getKartHeight    () const 
                   {return m_kart_properties->getKartModel()->getHeight();     }
    float          getWheelieAngle  () const {return m_wheelie_angle;          }
    btKart        *getVehicle       () const {return m_vehicle;                }
    btUprightConstraint *getUprightConstraint() const {return m_uprightConstraint;}
    void           createPhysics    ();
    void           draw             ();
    bool           isInRest         () const;
    //have to use this instead of moveable getVelocity to get velocity for bullet rigid body
    float          getSpeed         () const {return m_speed;                 }
    /** This is used on the client side only to set the speed of the kart
     *  from the server information.                                       */
    void           setSpeed         (float s) {m_speed = s;                   }
    void           setSuspensionLength();
    float          handleWheelie    (float dt);
    float          getActualWheelForce();
    bool           isOnGround       () const;
    bool           isEliminated     () const {return m_eliminated;}
    void           eliminate        ();
    bool           isRescue         () const {return m_rescue;}
    void           resetBrakes      ();
    void           adjustSpeed      (float f);
    void           updatedWeight    ();
    void           forceRescue      ();
    void           handleExplosion  (const Vec3& pos, bool direct_hit);
    const std::string& getName      () const {return m_kart_properties->getName();}
    const std::string& getIdent     () const {return m_kart_properties->getIdent();}
    virtual bool    isPlayerKart    () const {return false;                        }
    // addMessages gets called by world to add messages to the gui
    virtual void   addMessages      () {};
    virtual void   collectedItem    (const Item &item, int random_attachment);
    virtual void   reset            ();
    virtual void   handleZipper     ();
    virtual void   crashed          (Kart *k);
    
    virtual void   update           (float dt);
    virtual void   raceFinished     (float time);
    void           beep             ();
};


#endif

/* EOF */

