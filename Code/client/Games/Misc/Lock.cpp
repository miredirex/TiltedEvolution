#include <Games/Misc/Lock.h>

void Lock::SetLock(bool aIsLocked) noexcept
{
    TP_THIS_FUNCTION(TSetLock, void, Lock, bool);
    POINTER_SKYRIMSE(TSetLock, realSetLock, 12401);

    return TiltedPhoques::ThisCall(realSetLock, this, aIsLocked);
}
