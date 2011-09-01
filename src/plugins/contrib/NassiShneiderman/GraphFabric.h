
#ifndef __CreateGraphVisitor_h__
#define __CreateGraphVisitor_h__

#include "NassiBrickVisitor.h"
#include "GraphBricks.h"

class NassiView;
class GraphNassiBrick;
class GraphFabric  : public NassiBrickVisitor
{
public:
    GraphFabric(NassiView *view, BricksMap *map);
private:
    GraphFabric(const GraphFabric &p);
    GraphFabric &operator=(const GraphFabric &rhs);
public:
    virtual ~GraphFabric();
    //virtual void Visit(NassiBrick *brick);
    virtual void Visit(NassiInstructionBrick *brick);
    virtual void Visit(NassiIfBrick *brick);
    virtual void Visit(NassiWhileBrick *brick);
    virtual void Visit(NassiDoWhileBrick *brick);
    virtual void Visit(NassiSwitchBrick *brick);
    virtual void Visit(NassiBreakBrick *brick);
    virtual void Visit(NassiContinueBrick *brick);
    virtual void Visit(NassiReturnBrick *brick);
    virtual void Visit(NassiForBrick *brick);
    virtual void Visit(NassiBlockBrick *brick);
public:
    GraphNassiBrick *CreateGraphBrick(NassiBrick *brick);

private:
    GraphNassiBrick *gbrick;
    NassiView *m_view;
    BricksMap *m_map;
};

#endif //__CreateGraphVisitor_h__

