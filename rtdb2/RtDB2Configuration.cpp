#include "RtDB2Configuration.h"
#include "RtDB2ErrorCode.h"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <iostream>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

bool is_number(const std::string& s);
std::string to_upper(const std::string& s);
std::string to_lower(const std::string& s);

class CustomErrorHandler : public xercesc::HandlerBase {
public:
    CustomErrorHandler() { total_count = 0; error_count = 0; warning_count = 0; fatal_count = 0; }

    enum ErrorType { ERROR, FATAL_ERROR, WARNING };
    void create_error(ErrorType type, const std::string& error_msg) { handler(type, error_msg); }
    void error(const xercesc::SAXParseException& e) { handler(ERROR, e); }
    void fatalError(const xercesc::SAXParseException& e) { handler(FATAL_ERROR, e); }
    void warning(const xercesc::SAXParseException& e) { handler(WARNING, e); }

    int get_error_count() { return total_count; };
private:
    void handler(ErrorType error_type, const xercesc::SAXParseException& e) {
        char* message = xercesc::XMLString::transcode(e.getMessage());
        handler(error_type, message);
        xercesc::XMLString::release(&message);
    }
    void handler(ErrorType& type, const std::string& error_msg) {
        total_count++;
        switch (type) {
            case ERROR: std::cerr << "[ERROR] "; error_count++; break;
            case FATAL_ERROR: std::cerr << "[FATAL_ERROR] "; fatal_count++; break;
            case WARNING: std::cerr << "[WARNING] "; warning_count++; break;
        }
        std::cerr << error_msg << std::endl;
    }
    int error_count, fatal_count, warning_count, total_count;
};

RtDB2Configuration::RtDB2Configuration() {
    std::string configuration_path = "";;
    char *cp = NULL;
    if ((cp = getenv("RTDB_CONFIG_PATH")) != NULL)
    {
        configuration_path = cp;
    }
    else
    {
        throw std::runtime_error("Error while creating a configuration for the RTDB - No directory configured!");
    }
    std::string configuration_file = configuration_path + "/rtdb2_configuration.xml";
    int error = parse_configuration(configuration_file);
    if (error != RTDB2_SUCCESS)
        throw std::runtime_error("Error while creating a configuration for the RTDB - Failed to parse!");
}

int RtDB2Configuration::parse_configuration(std::string file_path) {
    try {
        xercesc::XMLPlatformUtils::Initialize();
    } catch(xercesc::XMLException& e) {
        char* message = xercesc::XMLString::transcode(e.getMessage());
        std::cerr << "XML toolkit initialization error: " << message << std::endl;
        xercesc::XMLString::release(&message);
        return RTDB2_FAILED_PARSING_CONFIG_FILE;
    }

    XMLCh* name_general = xercesc::XMLString::transcode("General");
    XMLCh* name_keys = xercesc::XMLString::transcode("Keys");
    XMLCh* name_oid = xercesc::XMLString::transcode("oid");
    XMLCh* name_shared = xercesc::XMLString::transcode("shared");
    XMLCh* name_period = xercesc::XMLString::transcode("period");
    XMLCh* name_phase = xercesc::XMLString::transcode("phase");
    XMLCh* name_timeout = xercesc::XMLString::transcode("timeout");
    XMLCh* name_id = xercesc::XMLString::transcode("id");

    boost::shared_ptr<xercesc::XercesDOMParser> config_parser = boost::make_shared<xercesc::XercesDOMParser>();
    boost::shared_ptr<CustomErrorHandler> err_handler = boost::make_shared<CustomErrorHandler>();

    // Check if the XML is well-formed
    config_parser->useScanner(xercesc::XMLUni::fgWFXMLScanner);
    config_parser->setErrorHandler(err_handler.get());

    try {
        // Parsing the XML if possible
        config_parser->parse(file_path.c_str());

        // Obtaining the document and the root node
        xercesc::DOMDocument* doc = config_parser->getDocument();
        xercesc::DOMElement* root = doc->getDocumentElement();

        if (!root) {
            err_handler->create_error(CustomErrorHandler::ERROR, "No root element was found");
            return RTDB2_FAILED_PARSING_CONFIG_FILE;
        }
        xercesc::DOMNodeList* general_list = root->getElementsByTagName(name_general);
        if (general_list->getLength() != 1) {
            err_handler->create_error(CustomErrorHandler::ERROR,
                                      "Tag \"General\" was not found or found more than once");
            return RTDB2_FAILED_PARSING_CONFIG_FILE;
        }
        // TAG <General>
        // Iterating over the General nodes
        xercesc::DOMNodeList* general_list_childs = general_list->item(0)->getChildNodes();
        for(XMLSize_t i = 0; i < general_list_childs->getLength(); i++) {
            xercesc::DOMNode *current_node = general_list_childs->item(i);
            // Skip Non-Element nodes
            if (!(current_node->getNodeType() && current_node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE))
                continue;
            xercesc::DOMElement* element = dynamic_cast<xercesc::DOMElement*>(current_node);
            std::string tag_name = xercesc::XMLString::transcode(element->getTagName());

            // If it is DefaultKeyValue, process it
            if (tag_name == "DefaultKeyValue") {
                xercesc::DOMAttr* attr_shared = element->getAttributeNode(name_shared);
                if (attr_shared == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute shared on tag DefaultKeyValue");
                    continue;
                }

                std::string shared = to_lower(xercesc::XMLString::transcode(attr_shared->getValue()));
                if (shared == "true" || shared == "1" || shared == "on") {
                    default_details_.shared = true;
                } else if (shared == "false" || shared == "0" || shared == "off") {
                    default_details_.shared = false;
                } else {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Shared attribute value in tag \"DefaultKeyValue\" is invalid.");
                }

                xercesc::DOMAttr* attr_period = element->getAttributeNode(name_period);
                if (attr_period == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute period on tag DefaultKeyValue");
                    continue;
                }
                std::string period = xercesc::XMLString::transcode(attr_period->getValue());
                if (!is_number(period)) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Attribute period on tag DefaultKeyValue is not a number");
                    continue;
                }
                default_details_.period = atoi(period.c_str());

                xercesc::DOMAttr* attr_phase = element->getAttributeNode(name_phase);
                if (attr_phase == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute phase on tag DefaultKeyValue");
                    continue;
                }
                std::string phase = xercesc::XMLString::transcode(attr_phase->getValue());
                if (!is_number(phase)) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Attribute phase on tag DefaultKeyValue is not a number");
                    continue;
                }
                default_details_.phase_shift = atoi(phase.c_str());

                xercesc::DOMAttr* attr_timeout = element->getAttributeNode(name_timeout);
                if (attr_timeout == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute timeout on tag DefaultKeyValue");
                    continue;
                }
                std::string timeout = xercesc::XMLString::transcode(attr_timeout->getValue());
                if (!is_number(timeout)) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Attribute timeout on tag DefaultKeyValue is not a number");
                    continue;
                }
                default_details_.timeout = atof(timeout.c_str());
            } // If it is Compressor, process it
            else if (tag_name == "Compressor") {
                XMLCh* name_name = xercesc::XMLString::transcode("name");
                XMLCh* name_dictionary = xercesc::XMLString::transcode("dictionary");
                xercesc::DOMAttr *attr_name = element->getAttributeNode(name_name);
                if (attr_name == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute name on tag Compressor - considering default");
                    continue;
                }
                compressor_settings_.name = to_lower(xercesc::XMLString::transcode(attr_name->getValue()));

                xercesc::DOMAttr *attr_dictionary = element->getAttributeNode(name_dictionary);
                if (attr_dictionary == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute dictionary on tag Compressor - considering default");
                    continue;
                }
                std::string use_dictionary = to_lower(xercesc::XMLString::transcode(attr_dictionary->getValue()));
                if (use_dictionary == "true" || use_dictionary == "1" || use_dictionary == "on") {
                    compressor_settings_.use_dictionary = true;
                } else if (use_dictionary == "false" || use_dictionary == "0" || use_dictionary == "off") {
                    compressor_settings_.use_dictionary = false;
                } else {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Shared attribute value in tag \"DefaultKeyValue\" is invalid.");
                }
            } else if (tag_name == "Communication") {
                xercesc::DOMAttr *attr = NULL;
                std::string s;
                
                attr = element->getAttributeNode(xercesc::XMLString::transcode("multiCastIP"));
                if (attr == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute 'multiCastIP' on tag Communication");
                    continue;
                } else {
                    communication_settings_.multiCastIP = xercesc::XMLString::transcode(attr->getValue());
                }

                communication_settings_.interface = "auto"; // optional overrule in xml
                attr = element->getAttributeNode(xercesc::XMLString::transcode("interface"));
                if (attr != NULL) {
                    communication_settings_.interface = xercesc::XMLString::transcode(attr->getValue());
                }

                attr = element->getAttributeNode(xercesc::XMLString::transcode("frequency"));
                if (attr == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute 'frequency' on tag Communication");
                    continue;
                } else {
                    s = xercesc::XMLString::transcode(attr->getValue());
                    try
                    {
                        communication_settings_.frequency = atof(s.c_str());
                    }
                    catch (...)
                    {
                        err_handler->create_error(CustomErrorHandler::WARNING,
                                                  "Attribute 'frequency' on tag Communication is not a number");
                        continue;
                    }
                }

                attr = element->getAttributeNode(xercesc::XMLString::transcode("port"));
                if (attr == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute 'port' on tag Communication");
                    continue;
                } else {
                    s = xercesc::XMLString::transcode(attr->getValue());
                    if (!is_number(s)) {
                        err_handler->create_error(CustomErrorHandler::WARNING,
                                                  "Attribute 'port' on tag Communication is not a number");
                        continue;
                    }
                    communication_settings_.port = atoi(s.c_str());
                }

                attr = element->getAttributeNode(xercesc::XMLString::transcode("compression"));
                if (attr == NULL) {
                    err_handler->create_error(CustomErrorHandler::WARNING,
                                              "Missing attribute 'compression' on tag Communication");
                    continue;
                } else {
                    s = xercesc::XMLString::transcode(attr->getValue());
                    if (s == "true" || s == "1") {
                        communication_settings_.compression = true;
                    } else if (s == "false" || s == "0") {
                        communication_settings_.compression = false;
                    } else {
                        err_handler->create_error(CustomErrorHandler::WARNING,
                                                  "Attribute 'compression' value is invalid");
                        continue;
                    }
                }

                communication_settings_.loopback = false;
                attr = element->getAttributeNode(xercesc::XMLString::transcode("loopback"));
                if (attr != NULL) {
                    s = xercesc::XMLString::transcode(attr->getValue());
                    if (s == "true" || s == "1" || s == "enabled") {
                        communication_settings_.loopback = true;
                    } else if (s == "false" || s == "0" || s == "disabled") {
                        communication_settings_.loopback = false;
                    } else {
                        err_handler->create_error(CustomErrorHandler::WARNING,
                                                  "Attribute 'loopback' value is invalid");
                        continue;
                    }
                }

                communication_settings_.send = true;
                attr = element->getAttributeNode(xercesc::XMLString::transcode("send"));
                if (attr != NULL) {
                    s = xercesc::XMLString::transcode(attr->getValue());
                    if (s == "true" || s == "1" || s == "enabled") {
                        communication_settings_.send = true;
                    } else if (s == "false" || s == "0" || s == "disabled") {
                        communication_settings_.send = false;
                    } else {
                        err_handler->create_error(CustomErrorHandler::WARNING,
                                                  "Attribute 'send' value is invalid");
                        continue;
                    }
                }
            } else {
                std::stringstream ss;
                ss << "Unknown tag \"";
                ss << tag_name << "\" was found";
                err_handler->create_error(CustomErrorHandler::WARNING, ss.str());
                continue;
            }
        }

        // TAG <Keys>
        // Iterating over the Keys nodes
        xercesc::DOMNodeList* keys_id = root->getElementsByTagName(name_keys);
        if (keys_id->getLength() != 1) {
            std::cerr << "Tag \"Keys\" was not found or found more than once" << std::endl;
            return RTDB2_FAILED_PARSING_CONFIG_FILE;
        }
        xercesc::DOMNodeList* keys_list = keys_id->item(0)->getChildNodes();
        for(XMLSize_t i = 0; i < keys_list->getLength(); i++) {
            xercesc::DOMNode* current_node = keys_list->item(i);
            KeyDetail detail(default_details_);
            // Skip Non-Element nodes
            if (!(current_node->getNodeType() && current_node->getNodeType() == xercesc::DOMNode::ELEMENT_NODE))
                continue;
            xercesc::DOMElement* element = dynamic_cast<xercesc::DOMElement*>(current_node);
            std::string tag_name = xercesc::XMLString::transcode(element->getTagName());
            if (tag_name != "key") {
                std::stringstream ss;
                ss << "Unknown tag \"";
                ss << tag_name << "\" was found";
                err_handler->create_error(CustomErrorHandler::WARNING, ss.str());
                continue;
            }

            xercesc::DOMAttr* attr_id = element->getAttributeNode(name_id);
            xercesc::DOMAttr* attr_shared = element->getAttributeNode(name_shared);
            xercesc::DOMAttr* attr_period = element->getAttributeNode(name_period);
            xercesc::DOMAttr* attr_phase = element->getAttributeNode(name_phase);
            xercesc::DOMAttr* attr_timeout = element->getAttributeNode(name_timeout);
            xercesc::DOMAttr* attr_oid = element->getAttributeNode(name_oid);

            // Check if the attribute nome and shared exist
            if (attr_id == NULL) {
                err_handler->create_error(CustomErrorHandler::WARNING, "Skipping a key - missing attribute id or shared");
                continue;
            }
            std::string id = to_upper(xercesc::XMLString::transcode(attr_id->getValue()));
            if (attr_oid != NULL) {
                std::string oid = xercesc::XMLString::transcode(attr_oid->getValue());
                if (!is_number(oid)) {
                    err_handler->create_error(CustomErrorHandler::WARNING, "oid from tag \"key\" is invalid");
                } else {
                    associate_keys_int_string(atoi(oid.c_str()), id);
                }
            }

            if (attr_shared != NULL) {
                std::string shared = to_lower(xercesc::XMLString::transcode(attr_shared->getValue()));
                if (shared == "true" || shared == "1" || shared == "on") {
                    detail.shared = true;
                } else if (shared == "false" || shared == "0" || shared == "off") {
                    detail.shared = false;
                } else {
                    err_handler->create_error(CustomErrorHandler::WARNING, "Shared attribute in tag \"key\" is invalid.");
                }
            }

            if (attr_period != NULL) {
                std::string period = xercesc::XMLString::transcode(attr_period->getValue());
                detail.period = atoi(period.c_str());
            }

            if (attr_phase != NULL) {
                std::string phase = xercesc::XMLString::transcode(attr_phase->getValue());
                detail.phase_shift = atoi(phase.c_str());
            }

            if (attr_timeout != NULL) {
                std::string timeout = xercesc::XMLString::transcode(attr_timeout->getValue());
                detail.timeout = atof(timeout.c_str());
            }
            insert_key_detail(id, detail);
        }
    } catch(xercesc::XMLException& e) {
        char* message = xercesc::XMLString::transcode(e.getMessage());
        std::ostringstream errBuf;
        errBuf << "Error parsing file: " << message << std::flush;
        xercesc::XMLString::release(&message);
        return RTDB2_FAILED_PARSING_CONFIG_FILE;
    }
    return err_handler->get_error_count() > 0 ? RTDB2_FAILED_PARSING_CONFIG_FILE : RTDB2_SUCCESS;
}

void RtDB2Configuration::associate_keys_int_string(int oid, std::string id) {
    reverse_key_map_.insert(std::pair<int, std::string>(oid, id));
}

void RtDB2Configuration::insert_key_detail(std::string id, KeyDetail detail) {
    keys_details_.insert(std::pair<std::string, KeyDetail>(id, detail));
}

const KeyDetail& RtDB2Configuration::get_key_default() const {
    return default_details_;
}

std::ostream& operator<<(std::ostream& os, RtDB2Configuration& obj) {
    os << "Associations between integer keys and string: " << std::endl;
    for (std::map<int, std::string>::iterator it = obj.reverse_key_map_.begin();
            it != obj.reverse_key_map_.end(); ++it) {
        os << "\tOID: " << it->first << ", ID: " << it->second << std::endl;
    }
    os << "Specific key configurations: " << std::endl;
    for (std::map<std::string, KeyDetail>::iterator it = obj.keys_details_.begin();
            it != obj.keys_details_.end(); ++it) {
        os << "\tKey: " << it->first
           << ", shared: " << it->second.shared
           << ", period: " << it->second.period
           << ", phase shift: " << it->second.phase_shift << std::endl;
    }
    os << "Defaults:" << std::endl;
    os << "\tShared: " << obj.default_details_.shared << std::endl;
    os << "\tPeriod: " << obj.default_details_.period << std::endl;
    os << "\tPhase shift: " << obj.default_details_.phase_shift << std::endl;
    return os;
}

const KeyDetail& RtDB2Configuration::get_key_details(const std::string& id) const {
    std::map<std::string, KeyDetail>::const_iterator it = keys_details_.find(id);
    if (it == keys_details_.end())
        return default_details_;
    return it->second;
}

const KeyDetail& RtDB2Configuration::get_key_details(const int& oid) const {
    std::map<int, std::string>::const_iterator it = reverse_key_map_.find(oid);
    if (it == reverse_key_map_.end())
        return default_details_;
    return get_key_details(it->second);
}

const std::string* RtDB2Configuration::get_string_identifier(const int& oid) const {
    std::map<int, std::string>::const_iterator it = reverse_key_map_.find(oid);
    if (it == reverse_key_map_.end())
        return NULL;
    return &it->second;
}

const CompressorSettings &RtDB2Configuration::get_compressor_settings() const {
    return compressor_settings_;
}

const CommunicationSettings &RtDB2Configuration::get_communication_settings() const {
    return communication_settings_;
}

std::string to_upper(const std::string& s) {
    std::string upper_string;
    std::string::const_iterator it = s.begin();
    while (it != s.end()) {
        upper_string += toupper(*it);
        ++it;
    }
    return upper_string;
}

std::string to_lower(const std::string& s) {
    std::string upper_string;
    std::string::const_iterator it = s.begin();
    while (it != s.end()) {
        upper_string += tolower(*it);
        ++it;
    }
    return upper_string;
}

bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (std::isdigit(*it) || *it=='.' || *it=='-')) ++it;
    return !s.empty() && it == s.end();
}
