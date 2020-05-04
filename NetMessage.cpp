#include "NetMsgCreateWO.h"
#ifdef AFTR_CONFIG_USE_BOOST

#include <iostream>
#include <sstream>
#include <string>
#include "AftrManagers.h"
#include "Vector.h"
#include "WO.h"
#include "GLView.h"
#include "WorldContainer.h"
#include "NetMessage.h"
#include "GLViewNewModule.h"
#include "Airplane.h"

using namespace Aftr;

NetMsgMacroDefinition(NetMessageCreateRawWO);

NetMessageCreateRawWO::NetMessageCreateRawWO(const Vector& pos) {
	this->pos = pos;
}

NetMessageCreateRawWO::~NetMessageCreateRawWO()
{
}

bool NetMessageCreateRawWO::toStream(NetMessengerStreamBuffer& os) const
{
	os << this->pos.x;
	os << this->pos.y;
	os << this->pos.z;
	return true;
}

bool NetMessageCreateRawWO::fromStream(NetMessengerStreamBuffer& is)
{
	is >> this->pos.x;
	is >> this->pos.y;
	is >> this->pos.z;
	return true;
}

void NetMessageCreateRawWO::onMessageArrived()
{
	((GLViewNewModule*)ManagerGLView::getGLView())->airplane->setPosition(this->pos.x, this->pos.y, this->pos.z);	
}

std::string NetMessageCreateRawWO::toString() const
{
	std::stringstream ss;

	ss << NetMsg::toString();
	ss << "Payload: Position = {" << this->pos.x << ", " << this->pos.y << ", " << this->pos.z << "}\n";

	return ss.str();
}

#endif