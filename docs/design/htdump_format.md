# HTDUMP format design {#design_htdump_format}
HTDUMP data format is a binary event format designed for efficient storage of data generated by %HawkTracer library. The format is very flexible in a way that user can define custom event types without modifying the source code of the event parser.  
As mentioned above, format allows user to define custom event classes. However, there are a few pre-defined classes which must be known for the parser in advance:

* `Event` - (see @ref htdump_format_base_event_class "Base event class")
* `EndiannessInfoEvent` - (see @ref htdump_format_endianness "Endianness")
* `EventKlassInfoEvent` - (see @ref htdump_format_custom_event_classes "Custom event classes")
* `EventKlassFieldInfoEvent` - (see @ref htdump_format_custom_event_classes "Custom event classes")

## @anchor htdump_format_base_event_class Base event class
HTDUMP format allows user to define custom event classes. Moreover, it allows to use user-defined class as a base class for other event class. However, all the event classes must directly or indirectly inherit from the Event class, which contains base data and all necessary information for parsing events of that class:
```c
Event {
	uint64 timestamp; // Timestamp in nanoseconds, when the event happened
	uint64 event_id;  // Unique event identifier
	uint32 klass_id;  // Identifier of an event class
}
```

Each event class has unique class identifier, which is used by parser to know how to parse a particular event. `timestamp` is used to determine when exactly the event happened in the system, and `event_id` is just a sequence number of the event (in case user needed to distinguish multiple events).

## Event stream structure
All the generated events are packed into the stream which then is parsed by the client application. Events in the stream can be in a different order (they don't have to be sorted by `event_id` or `timestamp`), however, there are 2 rules that every stream needs to follow:
* `EventKlassInfoEvent` and `EventKlassFieldInfoEvent` events of the particular event class must be placed in the stream before any instance of that class - parser reads the stream from the beginning to the end, and if there's an event of which the type is not known to the parser yet, parsing process will fail.
* `EndiannessInfoEvent` shoudl be the first event of the stream - if not, the parser will choose default endianness, which might not be correct for the stream.

## @anchor htdump_format_endianness Endianness
HawkTracer is intended to be used on various platforms with different CPUs, therefore at the very beginning of the event stream, the endianness of the platform must be specified. It's done by sending an event of EndiannessInfoEvent class:

```c
EndiannessInfoEvent {
	Event base;       // base class
	uint8 endianness; // endianness (little endian: 0, big_endian: 1)
}
```

The parser uses the `klass_id` field of the event to determine which klass description to use for parsing the event. However, before sending EndiannessInfoEvent parser doesn't know what the endianness is, so it's not possible to parse the field properly. To overcome the problem, EndiannessInfoEvent class uses `0` as class identifier, as it's the same value regardless of the endianness.

## @anchor htdump_format_custom_event_classes Custom event classes
To define custom event type, user needs to push the event class description to the event stream before pushing events of this class, so parser knows how to deal with this type of event.  
There are 2 event types that should be used to describe a new data type:

* `EventKlassInfoEvent` - contains information about the new event class:
```c
EventKlassInfoEvent {
	Event base;              // base class
	uint32 event_klass_id;   // id of the described class
	string event_klass_name; // name of the described class
	uint8 field_count;       // number of fields of the described class
}
```
* `EventKlassFieldInfoEvent` - describes each event class field separately. Number of events of this type for a particular event class shoudl match the value of `field_count` from `EventKlassInfoEvent` event.
```c
EventKlassFieldInfoEvent {
	Event base;             // base class
	uint32 event_klass_id;  // id of the field's class
	string field_type_name; // name of the field's data type
	string field_name;      // name of the field
	uint64 size;            // size of the field (in bytes)
	DataType data_type;     // a type of the field
}
```
The `DataType` is an enum, which is defined as follows:
```c
enum DataType(uint8)
{
	STRUCT,
	STRING,
	INTEGER,
	FLOAT,
	DOUBLE,
	POINTER,
	UNSIGNED_INTEGER
}
```

### Example
Assume we want to create an event class with following fields:
```c
typedef usage_t float;
CPUUsageEvent {
	Event base;
	string thread_name;
	usage_t usage;
	uint32_t cpu_num;
}
```
Before sending an event of this type to the event stream, there should be 4 events send before that:
```c
EventKlassInfoEvent {
	base: Event{},
	event_klass_id: get_class_id(CPUUsageEvent),
	event_klass_name: "CPUUsageEvent",
	field_count: 3
}

EventKlassFieldInfoEvent {
	base: Event{},
	event_klass_id: get_class_id(CPUUsageEvent),
	field_type_name: "string",
	field_name: "thread_name",
	size: 0, // not used for string type
	data_type: STRING
}

EventKlassFieldInfoEvent {
	base: Event{},
	event_klass_id: get_class_id(CPUUsageEvent),
	field_type_name: "usage_t",
	field_name: "usage",
	size: sizeof(usage_t),
	data_type: FLOAT
}

EventKlassFieldInfoEvent {
	base: Event{},
	event_klass_id: get_class_id(CPUUsageEvent),
	field_type_name: "uint32_t",
	field_name: "cpu_num",
	size: sizeof(uint32_t),
	data_type: UNSIGNED_INTEGER
}
```
