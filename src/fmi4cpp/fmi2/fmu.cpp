
#include <fmi4cpp/fmi2/fmu.hpp>
#include <fmi4cpp/fmi2/xml/model_description_parser.hpp>
#include <fmi4cpp/mlog.hpp>
#include <fmi4cpp/tools/os_util.hpp>
#include <fmi4cpp/tools/simple_id.hpp>
#include <fmi4cpp/tools/unzipper.hpp>

#include <utility>

using namespace fmi4cpp;
using namespace fmi4cpp::fmi2;

fmu::fmu()
{
}

fmu::fmu(const std::filesystem::path& fmuPath)
{
    load_model(fmuPath);
}

void fmu::load_model(const std::filesystem::path& fmuPath)
{
    if (!exists(fmuPath)) {
        const auto err = "No such file '" + absolute(fmuPath).string() + "'!";
        MLOG_FATAL(err);
        throw std::runtime_error(err);
    }

    MLOG_DEBUG("Loading FMU: " << fmuPath);

    const std::string fmuName = fmuPath.stem().string();

    std::filesystem::path targetPath;
    bool preExtractionFlag;
    if (std::filesystem::is_directory(fmuPath)) {
        MLOG_DEBUG("Attempting to load as pre-extracted fmu");
        preExtractionFlag = true;
        targetPath = fmuPath;
    } else {
        MLOG_DEBUG("Attempting to load as compressed fmu");
        preExtractionFlag = false;

        std::filesystem::path tmpPath(std::filesystem::temp_directory_path() /= std::filesystem::path("fmi4cpp_" + fmuName + "_" + generate_simple_id(8)));
        targetPath = tmpPath;

        if (!create_directories(tmpPath)) {
            const auto err = "Failed to create temporary directory '" + tmpPath.string() + "' !";
            MLOG_FATAL(err);
            throw std::runtime_error(err);
        }

        MLOG_DEBUG("Created temporary directory '" << tmpPath.string());

        if (!unzip(fmuPath, tmpPath.string())) {
            const auto err = "Failed to extract FMU '" + absolute(fmuPath).string() + "'!";
            MLOG_FATAL(err);
            throw std::runtime_error(err);
        }
    }

    resource_ = std::make_shared<fmu_resource>(targetPath);
    resource_->set_pre_extracted(preExtractionFlag);
    modelDescription_ = std::move(parse_model_description(resource_->model_description_path()));
}

std::string fmu::get_model_description_xml() const
{
    return resource_->get_model_description_xml();
}

std::shared_ptr<const model_description> fmu::get_model_description() const
{
    return modelDescription_;
}

bool fmu::supports_me() const
{
    return modelDescription_->supports_me();
}

bool fmu::supports_cs() const
{
    return modelDescription_->supports_cs();
}

std::unique_ptr<cs_fmu> fmu::as_cs_fmu() const
{
    std::shared_ptr<const cs_model_description> cs = std::move(modelDescription_->as_cs_description());
    return std::make_unique<cs_fmu>(resource_, cs);
}

std::unique_ptr<me_fmu> fmu::as_me_fmu() const
{
    std::shared_ptr<const me_model_description> me = std::move(modelDescription_->as_me_description());
    return std::make_unique<me_fmu>(resource_, me);
}
