#include "RtDB2Configuration.h"
#include "RtDB2ErrorCode.h"
#include "rtdb_configuration_generated.h"

#include <iostream>

std::string to_upper(const std::string &s);

RtDB2Configuration::RtDB2Configuration()
{
}

void RtDB2Configuration::load_configuration()
{
    std::string configuration_path = "";
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

int RtDB2Configuration::parse_configuration(std::string file_path)
{
    try
    {
        xml_schema::properties properties;
        // alternative for xsi:noNamespaceSchemaLocation definition in xml
        std::string xsd("rtdb_configuration.xsd");
        properties.no_namespace_schema_location(xsd);

        // TODO: Enable xsd validation. E.g. by embedding xsd in library:
        // https://stackoverflow.com/questions/4158900/embedding-resources-in-executable-using-gcc
        // https://stackoverflow.com/questions/11813271/embed-resources-eg-shader-code-images-into-executable-library-with-cmake
        ::std::unique_ptr<rtdbconfig::RtDBConfiguration> config(rtdbconfig::RtDB2Configuration(
            file_path, xml_schema::flags::dont_validate));

        bool parsed = false;
        // parse v2 style
        if (config->Networks().present())
        {
            parsed = true;
            for (rtdbconfig::Networks::Network_const_iterator network(config->Networks().get().Network().begin());
                 network != config->Networks().get().Network().end(); ++network)
            {
                // for now only provide support for the default network
                if (network->name().compare("default") == 0)
                {
                    communication_settings_.multiCastIP = network->MulticastAddress();
                    communication_settings_.frequency = network->Frequency();
                    communication_settings_.interface =
                        network->Interface().present() ? network->Interface().get() : "auto";
                    communication_settings_.loopback = network->loopback();
                    communication_settings_.port = network->MulticastPort();
                    communication_settings_.send = network->send();

                    if (network->Compression().present())
                    {
                        communication_settings_.compression = true;
                        switch (network->Compression().get().type())
                        {
                        case rtdbconfig::compressorType::zstd:
                            compressor_settings_.name = "zstd";
                            break;
                        case rtdbconfig::compressorType::lz4:
                            compressor_settings_.name = "lz4";
                            break;
                        default:
                            std::cerr << "[WARNING] Unsupported compressor type: " << network->Compression().get().type() << std::endl;
                            compressor_settings_.name = "zstd";
                        }
                        compressor_settings_.use_dictionary = network->Compression().get().UseDictionary();
                    }
                }
            }
        }
        if (config->Databases().present())
        {
            parsed = true;
            // for now only provide support for the default network
            for (rtdbconfig::Databases::Database_const_iterator database(config->Databases().get().Database().begin());
                 database != config->Databases().get().Database().end(); ++database)
            {
                if (database->name().compare("default") == 0)
                {
                    // DefaultKeyValue
                    bool present = database->KeyDefaults().present();
                    default_details_.period = present ? database->KeyDefaults().get().period() : rtdbconfig::keyDefaultsType::period_default_value();
                    default_details_.phase_shift = present ? database->KeyDefaults().get().phase() : rtdbconfig::keyDefaultsType::phase_default_value();
                    default_details_.shared = present ? database->KeyDefaults().get().shared() : rtdbconfig::keyDefaultsType::shared_default_value();
                    default_details_.timeout = present ? database->KeyDefaults().get().timeout() : rtdbconfig::keyDefaultsType::timeout_default_value();

                    // Keys
                    for (rtdbconfig::Keys::key_const_iterator key(database->Keys().Key().begin());
                         key != database->Keys().Key().end(); ++key)
                    {
                        KeyDetail kd;
                        kd.period = key->period().present() ? key->period().get() : default_details_.period;
                        kd.phase_shift = key->phase().present() ? key->phase().get() : default_details_.phase_shift;
                        kd.shared = key->shared().present() ? key->shared().get() : default_details_.shared;
                        kd.timeout = key->timeout().present() ? key->timeout().get() : default_details_.timeout;
                        insert_key_detail(to_upper(key->id()), kd);

                        if (key->oid().present())
                        {
                            associate_keys_int_string(key->oid().get(), to_upper(key->id()));
                        }
                    }
                }
            }
        }
        if (!parsed)
        {
            // parse v1 style
            if (config->General().present())
            {
                rtdbconfig::General general = config->General().get();
                communication_settings_.multiCastIP = general.Communication().multiCastIP();
                communication_settings_.compression = general.Communication().compression();
                communication_settings_.frequency = general.Communication().frequency();
                communication_settings_.interface = general.Communication().interface();
                communication_settings_.loopback = general.Communication().loopback();
                communication_settings_.port = general.Communication().port();
                communication_settings_.send = general.Communication().send();

                // Compressor settings
                switch (general.Compressor().name())
                {
                case rtdbconfig::compressorType::zstd:
                    compressor_settings_.name = "zstd";
                    break;
                case rtdbconfig::compressorType::lz4:
                    compressor_settings_.name = "lz4";
                    break;
                default:
                    std::cerr << "[WARNING] Unsupported compressor type: " << general.Compressor().name() << std::endl;
                    compressor_settings_.name = "zstd";
                }
                compressor_settings_.use_dictionary = general.Compressor().dictionary();

                // DefaultKeyValue
                default_details_.period = general.DefaultKeyValue().period();
                default_details_.phase_shift = general.DefaultKeyValue().phase();
                default_details_.shared = general.DefaultKeyValue().shared();
                default_details_.timeout = general.DefaultKeyValue().timeout();
            }
            // Keys
            if (config->Keys().present())
            {
                for (rtdbconfig::Keys::key_const_iterator key(config->Keys().get().key().begin());
                     key != config->Keys().get().key().end(); ++key)
                {
                    KeyDetail kd;
                    kd.period = key->period().present() ? key->period().get() : default_details_.period;
                    kd.phase_shift = key->phase().present() ? key->phase().get() : default_details_.phase_shift;
                    kd.shared = key->shared().present() ? key->shared().get() : default_details_.shared;
                    kd.timeout = key->timeout().present() ? key->timeout().get() : default_details_.timeout;
                    insert_key_detail(to_upper(key->id()), kd);

                    if (key->oid().present())
                    {
                        associate_keys_int_string(key->oid().get(), to_upper(key->id()));
                    }
                }
            }
        }
    }
    catch (const xml_schema::exception &e)
    {
        std::cerr << e << std::endl;
        return RTDB2_FAILED_PARSING_CONFIG_FILE;
    }

    return RTDB2_SUCCESS;
}

void RtDB2Configuration::associate_keys_int_string(int oid, std::string id)
{
    reverse_key_map_.insert(std::pair<int, std::string>(oid, id));
}

void RtDB2Configuration::insert_key_detail(std::string id, KeyDetail detail)
{
    keys_details_.insert(std::pair<std::string, KeyDetail>(id, detail));
}

const KeyDetail &RtDB2Configuration::get_key_default() const
{
    return default_details_;
}

std::ostream &operator<<(std::ostream &os, RtDB2Configuration &obj)
{
    os << "Associations between integer keys and string: " << std::endl;
    for (std::map<int, std::string>::iterator it = obj.reverse_key_map_.begin();
         it != obj.reverse_key_map_.end(); ++it)
    {
        os << "\tOID: " << it->first << ", ID: " << it->second << std::endl;
    }
    os << "Specific key configurations: " << std::endl;
    for (std::map<std::string, KeyDetail>::iterator it = obj.keys_details_.begin();
         it != obj.keys_details_.end(); ++it)
    {
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

const KeyDetail &RtDB2Configuration::get_key_details(const std::string &id) const
{
    std::map<std::string, KeyDetail>::const_iterator it = keys_details_.find(id);
    if (it == keys_details_.end())
        return default_details_;
    return it->second;
}

const KeyDetail &RtDB2Configuration::get_key_details(const int &oid) const
{
    std::map<int, std::string>::const_iterator it = reverse_key_map_.find(oid);
    if (it == reverse_key_map_.end())
        return default_details_;
    return get_key_details(it->second);
}

const std::string *RtDB2Configuration::get_string_identifier(const int &oid) const
{
    std::map<int, std::string>::const_iterator it = reverse_key_map_.find(oid);
    if (it == reverse_key_map_.end())
        return NULL;
    return &it->second;
}

const CompressorSettings &RtDB2Configuration::get_compressor_settings() const
{
    return compressor_settings_;
}

const CommunicationSettings &RtDB2Configuration::get_communication_settings() const
{
    return communication_settings_;
}

std::string to_upper(const std::string &s)
{
    std::string upper_string;
    std::string::const_iterator it = s.begin();
    while (it != s.end())
    {
        upper_string += toupper(*it);
        ++it;
    }
    return upper_string;
}
