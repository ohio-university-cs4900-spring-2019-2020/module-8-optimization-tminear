#pragma once

#include "Model.h"
#include "WO.h"

#ifdef AFTR_CONFIG_USE_BOOST

namespace Aftr
{

    class Airplane
    {
    public:
        static Airplane* New(Vector position = Vector(-25, 0, 5), const std::string file = "../mm/models/Aircraft_KC_135.3ds");

        void setPosition(float x, float y, float z) {
            airplane->setPosition(Vector(x, y, z));
        };
        void setPosition(Vector v) {
            airplane->setPosition(v);
        };
        Vector getPosition() {
            return airplane->getPosition();
        };
        WO* getWorldObject() {
            return airplane;
        };

    protected:
        WO* airplane;

        Airplane(Vector position = Vector(-25, 0, 5), const std::string file = "../mm/models/Aircraft_KC_135.3ds");
    };

}

#endif