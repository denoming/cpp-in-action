# Introduction

A `SingleProducerSequencer` is a synchronization primitive that can be used to coordinate access to a ring-buffer
for a single producer and one or more consumers.

A producer first acquires one (`claimOne` method) or up to given count (`claimUpTo` method) slots in a ring-buffer,
writes to the ring-buffer elements to those slots, and then finally publishes the values written to those slots.
A producer can never produce more than `bufferSize` elements in advance of where the consumer has consumed up to.

A consumer then waits for certain elements to be published, processes the items and then notifies the producer
when it has finished processing items by publishing the sequence number it has finished consuming in a `SequenceBarrier`
object.

# Dynamic behaviour

```plantuml
@startuml

participant Producer
participant SingleProducerSequencer
participant "SequenceBarrier\n[producer]" as SBP 
participant "SequenceBarrier\n[consumer]" as SBC
participant Consumer

Consumer -> SingleProducerSequencer : wait(seq: int = 0)
activate SingleProducerSequencer
SingleProducerSequencer -> SBP : wait(seq: int = 0)
deactivate SingleProducerSequencer
...
Producer -> SingleProducerSequencer : claimOne()
activate Producer
SingleProducerSequencer -> SBC : wait(seq: int = (0 - 16) = 240u)
activate SingleProducerSequencer
SingleProducerSequencer <-- SBC : wait(seq: int = 255 (lastPublished = initialValue))
note right: returns immediately
SingleProducerSequencer -> SingleProducerSequencer : claimPos++
note right: move claim position
Producer <-- SingleProducerSequencer : claimOne()
Producer -> Producer : write(index = 0)
Producer -> SingleProducerSequencer : publish(seq: int = 0)
deactivate Producer
SingleProducerSequencer -> SBP : publish(seq: int = 0)
SingleProducerSequencer <-- SBP : [resume] wait(seq: int = 0)
Consumer <-- SingleProducerSequencer : [resume] wait(seq: int = 0)
Consumer -> Consumer : read[index = 0]
Consumer -> Consumer : readPos++
note right: move read position
Consumer -> SBC : publish(seq: int = 0)
...
Producer -> SingleProducerSequencer : claimOne()
SingleProducerSequencer -> SBC : wait(seq: int = (1 - 16) = 241u)
SingleProducerSequencer <-- SBC : wait(seq: int = 0)
note right: returns immediately  and repeat above sequence
deactivate SingleProducerSequencer

@enduml
```