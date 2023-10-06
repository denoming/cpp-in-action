#pragma once

#include "SequenceTraits.hpp"
#include "SequenceRange.hpp"
#include "SequenceBarrier.hpp"

template<std::unsigned_integral TSequence = std::size_t,
         typename Traits = SequenceTraits<TSequence>>
class SingleProducerSequencer {
public:
    using Range = SequenceRange<TSequence, Traits>;

    SingleProducerSequencer(SequenceBarrier<TSequence, Traits>& consumerBarrier,
                            std::size_t bufferSize,
                            TSequence initialSeq = Traits::initialSequence)
        : _consumerBarrier{consumerBarrier}
        , _bufferSize{bufferSize}
        , _claimPos{TSequence(initialSeq + 1u)}
        , _producerBarrier{initialSeq}
    {
    }

    void
    close()
    {
        _producerBarrier.close();
        _consumerBarrier.close();
    }

    [[nodiscard]] io::awaitable<TSequence>
    claimOne()
    {
        io::cancellation_state cs = co_await io::this_coro::cancellation_state;
        if (auto slot = cs.slot(); slot.is_connected() and not slot.has_handler()) {
            slot.assign([this](auto) { close(); });
        }

        const std::unsigned_integral auto writePos = TSequence(_claimPos - _bufferSize);
        TSequence lastPublished = co_await _consumerBarrier.wait(writePos);
        co_return _claimPos++;
    }

    io::awaitable<Range>
    claimUpTo(std::size_t count)
    {
        io::cancellation_state cs = co_await io::this_coro::cancellation_state;
        if (auto slot = cs.slot(); slot.is_connected() and not slot.has_handler()) {
            slot.assign([this](auto) { close(); });
        }

        const std::unsigned_integral auto writePos = TSequence(_claimPos - _bufferSize);
        const TSequence maxSeq = TSequence(co_await _consumerBarrier.wait(writePos) + _bufferSize);

        const TSequence begin = _claimPos;
        const std::size_t maxCount = static_cast<std::size_t>(maxSeq - begin) + 1;
        count = std::min(count, maxCount);
        const TSequence end = static_cast<TSequence>(begin + count);

        _claimPos = end;
        co_return Range{begin, end};
    }

    void
    publish(TSequence seq)
    {
        _producerBarrier.publish(seq);
    }

    void
    publish(const Range& range)
    {
        _producerBarrier.publish(range.back());
    }

    [[nodiscard]] TSequence
    lastPublished() const
    {
        return _producerBarrier.lastPublished();
    }

    [[nodiscard]] io::awaitable<TSequence>
    wait(TSequence seq)
    {
        io::cancellation_state cs = co_await io::this_coro::cancellation_state;
        if (auto slot = cs.slot(); slot.is_connected() and not slot.has_handler()) {
            slot.assign([this](auto) { close(); });
        }

        co_return co_await _producerBarrier.wait(seq);
    }

private:
    SequenceBarrier<TSequence, Traits>& _consumerBarrier;
    const std::size_t _bufferSize;
    TSequence _claimPos;
    SequenceBarrier<TSequence, Traits> _producerBarrier;
};