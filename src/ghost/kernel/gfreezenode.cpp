#include "gfreezenode_p.h"
#include "gfreezenode_p_p.h"

// class GFreezeNode

GFreezeNode::GFreezeNode(QObject *parent)
    : GLeafNode(*new GFreezeNodePrivate(), parent)
{
    connect(this, &GGhostNode::statusChanged,
            [this](Ghost::Status status) {
        if (Ghost::Running == status) {
            emit started();
        } else if (Ghost::Success == status) {
            emit finished();
        }
    });
}

void GFreezeNode::setDuration(int value)
{
    Q_D(GFreezeNode);

    if (value < 0) {
        qWarning("GtGhost : Value is too small, reset to 0.");
        value = 0;
    }

    if (value != d->duration) {
        d->duration = value;
        emit durationChanged(value);
    }
}

int GFreezeNode::duration() const
{
    Q_D(const GFreezeNode);
    return d->duration;
}

// class GFreezeNodePrivate

GFreezeNodePrivate::GFreezeNodePrivate()
    : GLeafNodePrivate(Ghost::FreezeNode)
    , duration(1)
{
}

GFreezeNodePrivate::~GFreezeNodePrivate()
{
}

bool GFreezeNodePrivate::reset()
{
    return true;
}

void GFreezeNodePrivate::execute()
{
    Q_Q(GFreezeNode);

    Q_ASSERT(Ghost::Invalid != status);
    Q_ASSERT(Ghost::Running != status);

    if (duration > 0) {
        if (!timer) {
            timer = new QTimer(q);
            timer->setSingleShot(true);

            QObject::connect(timer.data(), &QTimer::timeout,
                             [this]() {
                Q_ASSERT(Ghost::Running == status);
                setStatus(Ghost::Success);
            });
        }

        timer->setInterval(duration);
        timer->start();

        setStatus(Ghost::Running);
    } else {
        setStatus(Ghost::Running);
        setStatus(Ghost::Success);
    }
}

bool GFreezeNodePrivate::terminate()
{
    if (timer) {
        timer->stop();
    }

    return true;
}
