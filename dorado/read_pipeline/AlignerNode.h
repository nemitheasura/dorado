#pragma once

#include "alignment/BedFile.h"
#include "alignment/IndexFileAccess.h"
#include "alignment/Minimap2Options.h"
#include "read_pipeline/ClientInfo.h"
#include "read_pipeline/MessageSink.h"
#include "utils/stats.h"
#include "utils/types.h"

#include <memory>
#include <string>
#include <vector>

struct bam1_t;
typedef struct mm_tbuf_s mm_tbuf_t;

namespace dorado {

namespace alignment {
class Minimap2Index;
}  // namespace alignment

class AlignerNode : public MessageSink {
public:
    AlignerNode(std::shared_ptr<alignment::IndexFileAccess> index_file_access,
                const std::string& index_file,
                const std::string& bed_file,
                const alignment::Minimap2Options& options,
                int threads);
    AlignerNode(std::shared_ptr<alignment::IndexFileAccess> index_file_access, int threads);
    ~AlignerNode() { stop_input_processing(); }
    std::string get_name() const override { return "AlignerNode"; }
    stats::NamedStats sample_stats() const override;
    void terminate(const FlushOptions&) override { stop_input_processing(); }
    void restart() override { start_input_processing(&AlignerNode::input_thread_fn, this); }

    alignment::HeaderSequenceRecords get_sequence_records_for_header() const;

private:
    void input_thread_fn();
    std::shared_ptr<const alignment::Minimap2Index> get_index(const ClientInfo& client_info);
    void align_read_common(ReadCommon& read_common, mm_tbuf_t* tbuf);
    void add_bed_hits_to_record(const std::string& genome, bam1_t* record);

    std::shared_ptr<const alignment::Minimap2Index> m_index_for_bam_messages{};
    std::vector<std::string> m_header_sequences_for_bam_messages{};
    std::shared_ptr<alignment::IndexFileAccess> m_index_file_access{};
    alignment::BedFile m_bed_file_for_bam_messages{};
};

}  // namespace dorado
