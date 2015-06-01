#include "grandomselectornode_p.h"
#include "grandomselectornode_p_p.h"

#include <QtCore/QLoggingCategory>

Q_LOGGING_CATEGORY(qlcRandomSelectorNode, "GtGhost.RandomSelectorNode")

// class GRandomSelectorNode

GRandomSelectorNode::GRandomSelectorNode(QObject *parent)
    : GCompositeNode(*new GRandomSelectorNodePrivate(), parent)
{
}

void GRandomSelectorNode::setUpdateMode(Ghost::UpdateMode value)
{
    Q_D(GRandomSelectorNode);

    if (value != d->updateMode) {
        d->updateMode = value;
        emit updateModeChanged(value);
    }
}

void GRandomSelectorNode::setRandomMode(Ghost::RandomMode value)
{
    Q_D(GRandomSelectorNode);

    if (value != d->randomMode) {
        d->randomMode = value;
        emit randomModeChanged(value);
    }
}

Ghost::UpdateMode GRandomSelectorNode::updateMode() const
{
    Q_D(const GRandomSelectorNode);
    return d->updateMode;
}

Ghost::RandomMode GRandomSelectorNode::randomMode() const
{
    Q_D(const GRandomSelectorNode);
    return d->randomMode;
}

// class GRandomSelectorNodePrivate

GRandomSelectorNodePrivate::GRandomSelectorNodePrivate()
    : GCompositeNodePrivate(Ghost::RandomSelectorNode)
    , updateMode(Ghost::ResetOnly)
    , randomMode(Ghost::Parity)
{
}

GRandomSelectorNodePrivate::~GRandomSelectorNodePrivate()
{
}

void GRandomSelectorNodePrivate::onChildStatusChanged(GGhostSourceNode *childNode)
{
    Q_ASSERT(Ghost::Invalid != status);

    Ghost::Status childStatus = childNode->status();

    if (Ghost::Stopped == childStatus) {
        setStatus(Ghost::Stopped);
    } else if (Ghost::Failure == childStatus) {
        executeNextChildNode();
    } else if (Ghost::Success == childStatus) {
        if (++unmatchCounter >= unmatchCount) {
            setStatus(Ghost::Success);
        } else {
            executeNextChildNode();
        }
    }
}

bool GRandomSelectorNodePrivate::initialize()
{
    sortedChildNodes = childNodes;

    resortChildNodes();

    if (GGhostNodePrivate::initialize(sortedChildNodes)) {
        setStatus(Ghost::StandBy);
        return true;
    }

    return false;
}

void GRandomSelectorNodePrivate::reset()
{
    Q_ASSERT(Ghost::Invalid != status);
    Q_ASSERT(Ghost::StandBy != status);
    Q_ASSERT(Ghost::Running != status);

    resortChildNodes();

    QListIterator<GGhostNode *> i(sortedChildNodes);

    i.toBack();
    while (i.hasPrevious()) {
        GGhostNode *childNode = i.previous();
        if (cast(childNode)->status != Ghost::StandBy) {
            cast(childNode)->reset();
        }
    }

    setStatus(Ghost::StandBy);
}

void GRandomSelectorNodePrivate::execute()
{
    Q_ASSERT(Ghost::Invalid != status);
    Q_ASSERT(Ghost::Running != status);

    setStatus(Ghost::Running);

    if (updateMode == Ghost::Everytime) {
        resortChildNodes();
    }

    unmatchCounter = 0;
    executeIndex = -1;
    executeCounter = 0;

    executeNextChildNode();
}

void GRandomSelectorNodePrivate::terminate()
{
    Q_ASSERT(Ghost::Running == status);

    GGhostNode *childNode
            = sortedChildNodes.at(executeIndex);
    cast(childNode)->terminate();
}

void GRandomSelectorNodePrivate::executeNextChildNode()
{
    bool r = true;

    while (++executeIndex < sortedChildNodes.count()) {
        GGhostNode *childNode = sortedChildNodes.at(executeIndex);
        GGhostNodePrivate *dptr = cast(childNode);
        if (dptr->callPrecondition()) {
            ++executeCounter;
            dptr->execute();
            r = false;
            break;
        }
    }

    if (r) {
        if (executeCounter) {
            setStatus(Ghost::Failure);
        } else {
            setStatus(breakStatus);
        }
    }
}

void GRandomSelectorNodePrivate::resortChildNodes()
{
    if (Ghost::Parity == randomMode) {
        Q_FOREACH (GGhostNode *childNode, childNodes) {
            cast(childNode)->sortIndex = qrand();
        }
    } else {
        Q_FOREACH (GGhostNode *childNode, childNodes) {
            cast(childNode)->sortIndex
                    = qrand() % cast(childNode)->callWeight();
        }
    }

    sort(sortedChildNodes);
}