
#ifndef __NASSI_BRICK_VISITOR_H__
#define __NASSI_BRICK_VISITOR_H__


#include "bricks.h"

class NassiBrickVisitor
{
public:
    NassiBrickVisitor()
    {}
    virtual ~NassiBrickVisitor()
    {}
    //virtual void Visit(NassiBrick *brick)=0;
    virtual void Visit(NassiInstructionBrick *brick)=0;
    virtual void Visit(NassiIfBrick *brick)=0;
    virtual void Visit(NassiWhileBrick *brick)=0;
    virtual void Visit(NassiDoWhileBrick *brick)=0;
    virtual void Visit(NassiSwitchBrick *brick)=0;
    virtual void Visit(NassiBreakBrick *brick)=0;
    virtual void Visit(NassiContinueBrick *brick)=0;
    virtual void Visit(NassiReturnBrick *brick)=0;
    virtual void Visit(NassiForBrick *brick)=0;
    virtual void Visit(NassiBlockBrick *brick)=0;
};




#endif
