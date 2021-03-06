#ifndef KEYTOOL_REQUEST_HPP
#define KEYTOOL_REQUEST_HPP

#include "uuid.hpp"
#include "utils.hpp"
#include "key-type.hpp"
#include "derivation-path.hpp"
#include "use-info.hpp"
#include <string>
#include <variant>
#include <optional>

class SeedRequestBody final {
public:
    explicit SeedRequestBody(const ByteVector& digest) : _digest(digest) { }

    const ByteVector& digest() const { return _digest; }

    void encode_cbor(ByteVector& cbor) const;
    static SeedRequestBody decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

private:
    ByteVector _digest;
};

class KeyRequestBody final {
public:
    KeyRequestBody(
        const KeyType& key_type,
        const DerivationPath& path,
        const UseInfo& use_info,
        bool is_derivable
    )
        : _key_type(key_type)
        , _path(path)
        , _use_info(use_info)
        , _is_derivable(is_derivable)
    { }

    const KeyType& key_type() const { return _key_type; }
    const DerivationPath& path() const { return _path; }
    const UseInfo& use_info() const { return _use_info; }
    bool is_derivable() const { return _is_derivable; }

    void encode_cbor(ByteVector& cbor) const;
    static KeyRequestBody decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

private:
    KeyType _key_type;
    DerivationPath _path;
    UseInfo _use_info;
    bool _is_derivable;
};

class PSBTSignatureRequestBody final {
public:
    PSBTSignatureRequestBody() { }

    void encode_cbor(ByteVector& cbor) const;
    static PSBTSignatureRequestBody decode_cbor(ByteVector::const_iterator& pos, ByteVector::const_iterator end);

private:
};

typedef std::variant<SeedRequestBody, KeyRequestBody, PSBTSignatureRequestBody> RequestBodyVariant;

class Request final {
public:
    explicit Request(RequestBodyVariant body, const std::string& description = "", const UUID& id = UUID());
    explicit Request(const std::string& ur);

    const RequestBodyVariant& body() const { return *_body; }
    const std::string& description() const { return _description; }
    const UUID& id() const { return *_id; }

    bool is_seed_request() const { return std::holds_alternative<SeedRequestBody>(body()); }
    bool is_key_request() const { return std::holds_alternative<KeyRequestBody>(body()); }
    bool is_psbt_signature_request() const { return std::holds_alternative<PSBTSignatureRequestBody>(body()); }

    const SeedRequestBody& seed_request() const { return std::get<SeedRequestBody>(body()); }
    const KeyRequestBody& key_request() const { return std::get<KeyRequestBody>(body()); }
    const PSBTSignatureRequestBody& psbt_signature_request() const { return std::get<PSBTSignatureRequestBody>(body()); }

    std::string ur() const;

private:
    std::optional<UUID> _id;
    std::string _description;
    std::optional<RequestBodyVariant> _body;
};

#endif
