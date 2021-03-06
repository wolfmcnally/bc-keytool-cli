#include "model-hdkey-request.hpp"
#include "model.hpp"
#include <optional>

using namespace std;

DataNode<string>* setup_key_request_description(Model& model) {
    auto node = new DataNode<string>();
    model.add_node(node);
    node->set_info("key-request-description", "TEXT", "An informational note about the request.");
    node->set_to_string([](const string& s) { return s; });
    node->set_from_string([](const string& s) -> string { return s; });
    model.add_derivation("key-request-description <- [key-request]");
    model.add_derivation("key-request-description (default: empty)");
    node->set_derivation([&]() -> optional<string> {
        if(model.key_request->has_assigned_value()) {
            return model.key_request->value().description();
        } else {
            return "";
        }
    });
    return node;
}

DataNode<KeyType>* setup_key_request_type(Model& model) {
    auto node = new DataNode<KeyType>();
    model.add_node(node);
    node->set_info("key-request-type", "ENUM private | public", "The type of key to be derived.");
    node->set_to_string([](const KeyType& key_type) { return key_type.name(); });
    node->set_from_string([](const string& name) -> KeyType { return KeyType::find(name); });
    model.add_derivation("key-request-type <- [key-request]");
    model.add_derivation("key-request-type (default: private)");
    node->set_derivation([&]() -> optional<KeyType> {
        if(model.key_request->has_assigned_value()) {
            return model.key_request->value().key_request().key_type();
        } else {
            return KeyType::private_key();
        }
    });
    return node;
}

DataNode<UUID>* setup_key_request_id(Model& model) {
    auto node = new DataNode<UUID>();
    model.add_node(node);
    node->set_info("key-request-id", "UUID", "The ID of the request and response.");
    node->set_to_string([](const UUID& uuid) { return uuid.str(); });
    node->set_from_string([](const string& s) -> UUID { return UUID(s); });
    model.add_derivation("key-request-id <- [key-request]");
    model.add_derivation("key-request-id <- [key-response]");
    model.add_derivation("key-request-id (default: unique)");
    node->set_derivation([&]() -> optional<UUID> {
        if(model.key_request->has_assigned_value()) {
            return model.key_request->value().id();
        // cppcheck-suppress internalAstError
        } else if(model.key_response->has_assigned_value()) {
            return model.key_response->value().id();
        } else {
            return UUID();
        }
    });
    return node;
}

DataNode<HDKey>* setup_source_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("source-key", "HDKEY", "The BIP-32 HD key from which to derive another.");
    node->set_to_string([](const HDKey& key) { return key.ur(); });
    node->set_from_string([](const string& ur) -> HDKey { return HDKey::from_ur(ur); });
    model.add_derivation("source-key <- [master-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.master_key->has_value()) {
            return model.master_key->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<DerivationPath>* setup_key_request_derivation_path(Model& model) {
    auto node = new DataNode<DerivationPath>();
    model.add_node(node);
    node->set_info("key-request-derivation-path", "BIP32_PATH", "The requested derivation steps.");
    node->set_to_string([](const DerivationPath& path) { return path.to_string(); });
    node->set_from_string([](const string& p) -> DerivationPath { return DerivationPath::from_string(p); });
    model.add_derivation("key-request-derivation-path <- [key-request]");
    model.add_derivation("key-request-derivation-path <- [full-address-derivation-path]");
    node->set_derivation([&]() -> optional<DerivationPath> {
        if(model.key_request->has_assigned_value()) {
            return model.key_request->value().key_request().path();
        } else if(model.full_address_derivation_path->has_value()) {
            return model.full_address_derivation_path->value();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Request>* setup_key_request(Model& model) {
    auto node = new DataNode<Request>();
    model.add_node(node);
    node->set_info("key-request", "REQUEST", "A request for a key with the given derivation.");
    node->set_to_string([](const Request& request) { return request.ur(); });
    node->set_from_string([](const string& s) -> Request { return Request(s); });
    model.add_derivation("key-request <- [key-request-derivation-path, key-request-type, key-request-id, key-request-description, asset, network, is-derivable]");
    node->set_derivation([&]() -> optional<Request> {
        if(model.key_request_derivation_path->has_value()) {
            auto key_type = model.key_request_type->value();
            auto path = model.key_request_derivation_path->value();
            auto asset = model.asset->value();
            auto network = model.network->value();
            auto use_info = UseInfo(asset, network);
            auto is_derivable = model.is_derivable->value();
            auto body = KeyRequestBody(key_type, path, use_info, is_derivable);
            auto description = model.key_request_description->value();
            auto id = model.key_request_id->value();
            return Request(body, description, id);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<Response>* setup_key_response(Model& model) {
    auto node = new DataNode<Response>();
    model.add_node(node);
    node->set_info("key-response", "RESPONSE", "A response containing the requested key.");
    node->set_to_string([](const Response& response) { return response.ur(); });
    node->set_from_string([](const string& s) -> Response {
        auto response = Response(s);
        if(!response.is_key_response()) {
            throw domain_error("Key-response can only contain a ur:crypto-response with a key.");
        }
        return response;
    });
    model.add_derivation("key-response <- [key-request, source-key]");
    node->set_derivation([&]() -> optional<Response> {
        if(model.key_request->has_value() && model.source_key->has_value()) {
            auto source_key = model.source_key->value();
            auto request = model.key_request->value();
            auto request_id = request.id();
            auto request_body = request.key_request();
            auto path = request_body.path();
            auto key_type = request_body.key_type();
            auto use_info = request_body.use_info();
            auto is_derivable = request_body.is_derivable();
            if(auto source_fingerprint = path.source_fingerprint()) {
                if(*source_fingerprint != source_key.key_fingerprint()) {
                    throw domain_error("Source key does not have same fingerprint as the one in the request.");
                }
            }
            auto derived_key = source_key.derive(key_type, path, is_derivable);
            return Response(derived_key, request_id);
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<HDKey>* setup_derived_key(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("derived-key", "HDKEY", "The derived key.");
    node->set_to_string([](const HDKey& key) { return key.ur(); });
    node->set_from_string([](const string& ur) -> HDKey { return HDKey::from_ur(ur); });
    model.add_derivation("derived-key <- [key-response]");
    node->set_derivation([&]() -> optional<HDKey> {
        if(model.key_response->has_value()) {
            return model.key_response->value().key_response();
        } else {
            return nullopt;
        }
    });
    return node;
}

DataNode<HDKey>* setup_derived_key_base58(Model& model) {
    auto node = new DataNode<HDKey>();
    model.add_node(node);
    node->set_info("derived-key-base58", "BASE58", "The derived key in Base58 format.");
    node->set_to_string([](const HDKey& key) { return key.to_base58(); });
    model.add_derivation("derived-key-base58 <- [derived-key]");
    node->set_derivation([&]() -> optional<HDKey> {
        return model.derived_key->optional_value();
    });
    return node;
}
