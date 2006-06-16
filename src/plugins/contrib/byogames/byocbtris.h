#ifndef BYOCBTRIS_H
#define BYOCBTRIS_H

#include "byogame.h"

/** \brief Clone of popular Tetr*s game */
class byoCBTris : public byoGame
{
    public:

        byoCBTris();
        virtual ~byoCBTris();

    protected:

        virtual void Play();
};

#endif
