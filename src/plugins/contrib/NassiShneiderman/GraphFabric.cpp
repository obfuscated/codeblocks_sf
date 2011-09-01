
#include "GraphFabric.h"
#include "bricks.h"
#include "GraphBricks.h"

GraphFabric::GraphFabric(NassiView *view, BricksMap *map):
    NassiBrickVisitor(),
    gbrick(0),
    m_view(view),
    m_map(map)
{}

GraphNassiBrick *GraphFabric::CreateGraphBrick(NassiBrick *brick)
{
    if ( brick )
    {
        brick->accept(this);
        return gbrick;
    }
    return 0;
}

GraphFabric::~GraphFabric(){}

void GraphFabric::Visit(NassiInstructionBrick *brick)
{
    gbrick = new GraphNassiInstructionBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiIfBrick *brick)
{
    gbrick = new GraphNassiIfBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiWhileBrick *brick)
{
    gbrick = new GraphNassiWhileBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiDoWhileBrick *brick)
{
    gbrick = new GraphNassiDoWhileBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiSwitchBrick *brick)
{
    gbrick = new GraphNassiSwitchBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiBreakBrick *brick)
{
    gbrick = new GraphNassiBreakBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiContinueBrick *brick)
{
    gbrick = new GraphNassiContinueBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiReturnBrick *brick)
{
    gbrick = new GraphNassiReturnBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiForBrick *brick)
{
    gbrick = new GraphNassiForBrick(m_view, brick, m_map);
}
void GraphFabric::Visit(NassiBlockBrick *brick)
{
    gbrick = new GraphNassiBlockBrick(m_view, brick, m_map);
}

