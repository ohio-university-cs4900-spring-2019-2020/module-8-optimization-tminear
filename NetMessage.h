#pragma once

#include "NetMsg.h"
#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr
{

	class NetMessageCreateRawWO : public NetMsg
	{
	public:
		NetMsgMacroDeclaration(NetMessageCreateRawWO);

		NetMessageCreateRawWO(const Vector& pos = Vector(0,0,0));
		virtual ~NetMessageCreateRawWO();
		virtual bool toStream(NetMessengerStreamBuffer& os) const;
		virtual bool fromStream(NetMessengerStreamBuffer& is);
		virtual void onMessageArrived();
		virtual std::string toString() const;

	protected:
		Aftr::Vector pos;
	};
}
#endif