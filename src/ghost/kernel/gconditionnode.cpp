#include "gconditionnode_p.h"
#include "gconditionnode_p_p.h"

// class GConditionNode

GConditionNode::GConditionNode(QObject *parent)
    : GLeafNode(*new GConditionNodePrivate(), parent)
{
    connect(this, &GGhostNode::statusChanged,
            [this](Ghost::Status status) {
        if (Ghost::Success == status) {
            emit passed();
        } else if (Ghost::Failure == status) {
            emit unpassed();
        }
    });
}

void GConditionNode::setCondition(const QJSValue &value)
{
    Q_D(GConditionNode);

    if (!d->condition.equals(value)) {
        d->condition = value;
        emit conditionChanged(value);
    }
}

QJSValue GConditionNode::condition() const
{
    Q_D(const GConditionNode);
    return d->condition;
}

// class GConditionNodePrivate

GConditionNodePrivate::GConditionNodePrivate()
    : GLeafNodePrivate(Ghost::ConditionNode)
{
}

GConditionNodePrivate::~GConditionNodePrivate()
{
}

bool GConditionNodePrivate::reset()
{
    return true;
}

void GConditionNodePrivate::execute()
{
    Q_ASSERT(Ghost::Invalid != status);
    Q_ASSERT(Ghost::Running != status);

    setStatus(Ghost::Running);

    if (callCondition()) {
        setStatus(Ghost::Success);
    } else {
        setStatus(Ghost::Failure);
    }
}

bool GConditionNodePrivate::terminate()
{
    return true;
}

bool GConditionNodePrivate::callCondition()
{
    QJSValue value = condition;

    if (condition.isCallable()) {
        value = condition.call();
    }

    if (value.isBool()) {
        return value.toBool();
    } else if (value.isError()) {
        qDebug("GtGhost : %s", qPrintable(value.toString()));
    }

    return false;
}
