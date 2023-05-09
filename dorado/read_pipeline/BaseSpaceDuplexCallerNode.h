#pragma once
#include "ReadPipeline.h"
#include "utils/bam_utils.h"

namespace dorado {
// Duplex caller node receives a map of template_id to complement_id (typically generated from a pairs file),
// and a map of `read_id` to `dorado::Read` object. It then performs duplex calling and pushes `dorado::Read`
// objects to its output queue.
class BaseSpaceDuplexCallerNode : public MessageSink {
public:
    BaseSpaceDuplexCallerNode(MessageSink& sink,
                              std::map<std::string, std::string> template_complement_map,
                              utils::read_map reads,
                              size_t threads);
    ~BaseSpaceDuplexCallerNode();

private:
    void worker_thread();
    void basespace(std::string template_read_id, std::string complement_read_id);
    MessageSink&
            m_sink;  // MessageSink to consume Duplex Called Reads. This will typically be a writer node
    size_t m_num_worker_threads{1};
    std::unique_ptr<std::thread> m_worker_thread;
    std::map<std::string, std::string> m_template_complement_map;
    utils::read_map m_reads;
};
}  // namespace dorado
