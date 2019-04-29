#pragma once
#include "serialized_object.h"

#include "event_types.h"

class Event : public SerializedObject
{
  public:
    Event() : Event((QuesyncEvent)0)
    {
    }

    Event(QuesyncEvent evt)
    {
        _json["event_type"] = evt;
    }

    GET_FUNCTION(event_type, QuesyncEvent)
};