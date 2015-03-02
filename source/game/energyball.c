#include <stdio.h>
#include <string.h>
#include <3ds.h>
#include "game/energyball.h"
#include "gfx/gs.h"

#define ENERGYBALLSIZE (128)

energyDevice_s energyDevice[NUMENERGYDEVICES];
energyBall_s energyBall[NUMENERGYBALLS];

md2_model_t energyCatcherModel, energyLauncherModel, energyBallModel;
texture_s energyLauncherTexture, energyLauncherActiveTexture, energyBallTexture;

const vect3Df_s energyDeviceDirection[6]={(vect3Df_s){1.0f,0,0},(vect3Df_s){-1.0f,0,0},(vect3Df_s){0,1.0f,0},(vect3Df_s){0,-1.0f,0},(vect3Df_s){0,0,1.0f},(vect3Df_s){0,0,-1.0f}};

void initEnergyBalls(void)
{
	int i;
	for(i=0;i<NUMENERGYDEVICES;i++)
	{
		energyDevice[i].used=false;
		energyDevice[i].id=i;
	}
	for(i=0;i<NUMENERGYBALLS;i++)
	{
		energyBall[i].used=false;
		energyBall[i].id=i;
	}
	
	md2ReadModel(&energyCatcherModel, "ballcatcher.md2");
	md2ReadModel(&energyLauncherModel, "balllauncher.md2");
	md2ReadModel(&energyBallModel, "energyball.md2");

	textureLoad(&energyLauncherTexture, "balllauncher.png", GPU_TEXTURE_MAG_FILTER(GPU_LINEAR)|GPU_TEXTURE_MIN_FILTER(GPU_LINEAR), 0);
	textureLoad(&energyLauncherActiveTexture, "balllauncheractive.png", GPU_TEXTURE_MAG_FILTER(GPU_LINEAR)|GPU_TEXTURE_MIN_FILTER(GPU_LINEAR), 0);
	textureLoad(&energyBallTexture, "energyball.png", GPU_TEXTURE_MAG_FILTER(GPU_LINEAR)|GPU_TEXTURE_MIN_FILTER(GPU_LINEAR), 0);
}

void exitEnergyBalls(void)
{
	md2FreeModel(&energyCatcherModel);
	md2FreeModel(&energyLauncherModel);
	md2FreeModel(&energyBallModel);

	textureFree(&energyLauncherTexture);
	textureFree(&energyBallTexture);
}

void initEnergyDevice(room_s* r, energyDevice_s* ed, vect3Di_s pos, deviceOrientation_t or, bool type)
{
	if(!ed)return;
	
	// initActivator(&ed->activator);
	md2InstanceInit(&ed->modelInstance, type?(&energyLauncherModel):(&energyCatcherModel), &energyLauncherTexture);
	ed->orientation=or;
	
	ed->surface=NULL;
	
	{//for collisions
		rectangle_s rec;
		rectangle_s* recp;
		rec.material=NULL;
		
			if(or!=mY)
			{
				rec.position=vaddi(pos,vect3Di(-1,4,-1));
				rec.size=vect3Di(2,0,2);
				rec.normal=vect3Df(0,1.0f,0);
				recp=addRoomRectangle(r, rec);
				if(recp)recp->hide=true;
				if(or==pY)ed->surface=recp;
			}
			if(or!=pY)
			{
				rec.position=vaddi(pos,vect3Di(1,-4,1));
				rec.size=vect3Di(-2,0,-2);
				rec.normal=vect3Df(0,1.0f,0);
				recp=addRoomRectangle(r, rec);
				if(recp)recp->hide=true;
				if(or==mY)ed->surface=recp;
			}
			if(or!=pZ)
			{
				rec.position=vaddi(pos,vect3Di(-1,-4,-1));
				rec.size=vect3Di(2,8,0);
				rec.normal=vect3Df(0,0,-1.0f);
				recp=addRoomRectangle(r, rec);
				if(recp)recp->hide=true;
				if(or==mZ)ed->surface=recp;
			}
			if(or!=mZ)
			{
				rec.position=vaddi(pos,vect3Di(-1,4,1));
				rec.size=vect3Di(2,-8,0);
				rec.normal=vect3Df(0,0,1.0f);
				recp=addRoomRectangle(r, rec);
				if(recp)recp->hide=true;
				if(or==pZ)ed->surface=recp;
			}
			if(or!=pX)
			{
				rec.position=vaddi(pos,vect3Di(-1,-4,-1));
				rec.size=vect3Di(0,8,2);
				rec.normal=vect3Df(-1.0f,0,0);
				recp=addRoomRectangle(r, rec);
				if(recp)recp->hide=true;
				if(or==mX)ed->surface=recp;
			}
			if(or!=mX)
			{
				rec.position=vaddi(pos,vect3Di(1,4,-1));
				rec.size=vect3Di(0,-8,2);
				rec.normal=vect3Df(1.0f,0,0);
				recp=addRoomRectangle(r, rec);
				if(recp)recp->hide=true;
				if(or==pX)ed->surface=recp;
			}
	}
	// pos=vect3Di(pos.x+r->position.x, pos.y, pos.z+r->position.y);
	ed->position=convertRectangleVector(pos);
	
	ed->active=type;
	ed->type=type;
	ed->used=true;
}

energyDevice_s* createEnergyDevice(room_s* r, vect3Di_s pos, deviceOrientation_t or, bool type)
{
	if(!r)return NULL;
	int i;
	for(i=0;i<NUMENERGYDEVICES;i++)
	{
		if(!energyDevice[i].used)
		{
			initEnergyDevice(r, &energyDevice[i],pos,or,type);
			return &energyDevice[i];
		}
	}
	return NULL;
}

void drawEnergyDevice(energyDevice_s* ed)
{
	if(!ed)return;

	gsPushMatrix();
		md2StartDrawing();

		gsTranslate(ed->position.x, ed->position.y, ed->position.z);
		
		switch(ed->orientation)
		{
			case mY:
				gsRotateX(-M_PI);
				break;
			case pX:
				gsRotateZ(M_PI/2);
				break;
			case mX:
				gsRotateZ(-M_PI/2);
				break;
			case pZ:
				gsRotateX(-M_PI/2);
				break;
			case mZ:
				gsRotateX(M_PI/2);
				break;
			default:
				break;
		}

		md2InstanceDraw(&ed->modelInstance);
	gsPopMatrix();
}

void drawEnergyDevices(void)
{
	int i;
	for(i=0;i<NUMENERGYDEVICES;i++)
	{
		if(energyDevice[i].used)drawEnergyDevice(&energyDevice[i]);
	}
}

void updateEnergyDevice(energyDevice_s* ed)
{
	if(!ed)return;
	
	if(ed->type)
	{
		//launcher
		if(ed->active)
		{
			md2InstanceChangeAnimation(&ed->modelInstance,1,true);
			createEnergyBall(ed, vaddf(ed->position, vdivf(energyDeviceDirection[ed->orientation],8)), energyDeviceDirection[ed->orientation], 300);
			ed->active=false;
		}
	}else{
		//receiver
		if(ed->active)
		{
			ed->modelInstance.texture=&energyLauncherActiveTexture;
			// useActivator(&ed->activator);
		}
	}
	
	md2InstanceUpdate(&ed->modelInstance);
}

void updateEnergyDevices(void)
{
	int i;
	for(i=0;i<NUMENERGYDEVICES;i++)
	{
		if(energyDevice[i].used)updateEnergyDevice(&energyDevice[i]);
	}
}

void initEnergyBall(energyBall_s* eb, energyDevice_s* ed, vect3Df_s pos, vect3Df_s dir, u16 life)
{
	if(!eb)return;
	
	md2InstanceInit(&eb->modelInstance, &energyBallModel, &energyBallTexture);
	
	eb->maxLife=life;
	eb->life=life;
	eb->launcher=ed;
	eb->position=pos;
	eb->direction=dir;
	eb->speed=0.3;
	
	eb->used=true;
}

energyBall_s* createEnergyBall(energyDevice_s* launcher, vect3Df_s pos, vect3Df_s dir, u16 life)
{
	int i;
	for(i=0;i<NUMENERGYBALLS;i++)
	{
		if(!energyBall[i].used)
		{
			initEnergyBall(&energyBall[i],launcher,pos,dir,life);
			return &energyBall[i];
		}
	}
	return NULL;
}

void drawEnergyBall(energyBall_s* eb)
{
	if(!eb)return;

	// TODO : decaying alpha

	gsPushMatrix();
		md2StartDrawing();

		float angle=(eb->modelInstance.currentFrame*1.0f+eb->modelInstance.interpolation);

		gsTranslate(eb->position.x, eb->position.y, eb->position.z);
		
		gsRotateX(angle*0.4);gsRotateY(angle*0.2);gsRotateZ(angle*0.1);

		md2InstanceDraw(&eb->modelInstance);
	gsPopMatrix();
}

void drawEnergyBalls(void)
{
	int i;
	for(i=0;i<NUMENERGYBALLS;i++)
	{
		if(energyBall[i].used)drawEnergyBall(&energyBall[i]);
	}
}

// void warpEnergyBall(portal_struct* p, energyBall_s* eb)
// {
// 	if(!p->targetPortal)return;
// 	eb->position=vaddf(warpVector(p,vectDifference(eb->position,p->position)),p->targetPortal->position);
// 	eb->direction=warpVector(p,eb->direction);
// }

void killEnergyBall(energyBall_s* eb)
{
	if(!eb)return;
	
	if(eb->launcher)eb->launcher->active=true;
	eb->used=false;
}

energyDevice_s* isEnergyCatcherSurface(rectangle_s* rec)
{
	if(!rec)return NULL;
	
	int i;
	for(i=0;i<NUMENERGYDEVICES;i++)
	{
		if(energyDevice[i].used && !energyDevice[i].type && energyDevice[i].surface==rec)return &energyDevice[i];
	}
	return NULL;
}

void updateEnergyBall(energyBall_s* eb)
{
	if(!eb)return;
	
	// vect3Df_s l=vsubf(eb->position,convertSize(vect(p->currentRoom->position.x,0,p->currentRoom->position.y)));
	
	// vect3Df_s ip=l, normal;
	// rectangle_s* col=collideGridCell(getCurrentCell(p->currentRoom,eb->position), eb->launcher?eb->launcher->surface:NULL, l, eb->direction, eb->speed, &ip, &normal);
	// if(!col)col=collideGridCell(getCurrentCell(p->currentRoom,vaddf(eb->position,vmulf(eb->direction,eb->speed))), eb->launcher?eb->launcher->surface:NULL, l, eb->direction, eb->speed, &ip, &normal);
	// if(col)
	// {
	// 	// NOGBA("COL COL COL %d",col->collides);
	// 	ip=vaddf(convertSize(vect(p->currentRoom->position.x,0,p->currentRoom->position.y)),ip);
	// 	energyDevice_s* ed=isEnergyCatcherSurface(col);
	// 	if(ed && !ed->active)
	// 	{
	// 		//caught
	// 		md2InstanceChangeAnimation(&ed->modelInstance,2,false);
	// 		md2InstanceChangeAnimation(&ed->modelInstance,1,true);
	// 		killEnergyBall(eb);
	// 		if(eb->launcher)eb->launcher->active=false;
	// 		ed->active=true;
	// 		return;
	// 	}

	// 	// vect3Df_s v=vect(0,0,0);
	// 	// int32 x, y, z;
	// 	// portal_struct* portal=NULL;
	// 	// if(isPointInPortal(&portal1,ip,&v,&x,&y,&z))portal=&portal1;
	// 	// if(abs(z)>=32)portal=NULL;
	// 	// if(!portal)
	// 	// {
	// 	// 	if(isPointInPortal(&portal2,ip,&v,&x,&y,&z))portal=&portal2;
	// 	// 	if(abs(z)>=32)portal=NULL;
	// 	// }
	// 	// if((portal && !portal->used) || (portal && portal->targetPortal && !portal->targetPortal->used))portal=NULL;
	// 	// if(!portal)
	// 	// {
	// 	// 	eb->position=ip;
	// 	// 	eb->direction=vsubf(eb->direction,vmulf(normal,2*dotProduct(eb->direction,normal)));
	// 	// 	eb->position=vaddf(eb->position,vmulf(eb->direction,ENERGYBALLSIZE));
	// 	// }else{
	// 	// 	eb->position=vaddf(eb->position,vmulf(eb->direction,eb->speed));
	// 	// 	warpEnergyBall(portal,eb);
	// 	// 	eb->position=vaddf(eb->position,vmulf(eb->direction,eb->speed));
	// 	// }

	// }else{
		eb->position=vaddf(eb->position,vmulf(eb->direction,eb->speed));
	// }
	
	md2InstanceUpdate(&eb->modelInstance);
	
	if(!eb->life)killEnergyBall(eb);
	else eb->life--;
}

void updateEnergyBalls(void)
{
	int i;
	for(i=0;i<NUMENERGYBALLS;i++)
	{
		if(energyBall[i].used)updateEnergyBall(&energyBall[i]);
	}
}
