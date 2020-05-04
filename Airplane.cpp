#include "Airplane.h"

#ifdef AFTR_CONFIG_USE_BOOST

using namespace Aftr;

Airplane* Airplane::New(Vector position, const std::string file) {
    return new Airplane(position, file);
}

Airplane::Airplane(Vector position, const std::string file) {
    this->airplane = WO::New(file, Vector(0.01, 0.01, 0.01), MESH_SHADING_TYPE::mstFLAT);
    this->airplane->setPosition(position);
}

#endif