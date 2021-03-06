#ifndef KEYTOOL_DERIVATION_PATH_2
#define KEYTOOL_DERIVATION_PATH_2

#include "derivation-step.hpp"
#include <vector>
#include <optional>

class DerivationPath final {
public:
    DerivationPath(
        const std::vector<DerivationStep>& steps = {},
        std::optional<uint32_t> source_fingerprint = std::nullopt,
        std::optional<uint8_t> depth = std::nullopt
    );

    // Denotes just the fingerprint of a master key.
    explicit DerivationPath(uint32_t source_fingerprint);

    const std::vector<DerivationStep>& steps() const { return _steps; }
    std::optional<uint32_t> source_fingerprint() const { return _source_fingerprint; }
    std::optional<uint8_t> depth() const { return _depth; }
    uint8_t effective_depth() const;

    static DerivationPath decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    static DerivationPath decode_tagged_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);
    void encode_cbor(ByteVector& cbor) const;
    void encode_tagged_cbor(ByteVector& cbor) const;

    std::string to_string() const;
    static DerivationPath from_string(const std::string& path);

    void append(const DerivationPath& other);

private:
    std::vector<DerivationStep> _steps;
    std::optional<uint32_t> _source_fingerprint;
    std::optional<uint8_t> _depth;

    static DerivationStep parse_elem(const std::string& elem);
};

std::ostream& operator<< (std::ostream& os, const DerivationPath& rhs);

#endif
