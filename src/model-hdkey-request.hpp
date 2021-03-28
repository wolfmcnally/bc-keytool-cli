#ifndef KEYTOOL_MODEL_HDKEY_REQUEST_HPP
#define KEYTOOL_MODEL_HDKEY_REQUEST_HPP

#include "data-node.hpp"
#include "uuid.hpp"
#include "request.hpp"
#include "response.hpp"
#include "key-type.hpp"
#include "hd-key-2.hpp"
#include "derivation-path-2.hpp"

class Model;

DataNode<std::string>* setup_key_request_description(Model& model);
DataNode<KeyType>* setup_key_request_type(Model& model);
DataNode<UUID>* setup_key_request_id(Model& model);
DataNode<HDKey2>* setup_source_key(Model& model);
DataNode<DerivationPath2>* setup_key_request_derivation_path(Model& model);
DataNode<Request>* setup_key_request(Model& model);
DataNode<Response>* setup_key_response(Model& model);
DataNode<HDKey2>* setup_derived_key(Model& model);

#endif