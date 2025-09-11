#include "utils/tui/gnome.hpp"

static std::string exec_capture(const std::string &cmd)
{
    std::array<char, 256> buffer{};
    std::string result;
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        return "";
    while (fgets(buffer.data(), (int)buffer.size(), pipe) != nullptr)
    {
        result += buffer.data();
    }
    pclose(pipe);
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' '))
        result.pop_back();
    return result;
}

static int exec_nowait(const std::string &cmd)
{
    return system(cmd.c_str());
}

static bool command_exists(const std::string &name)
{
    std::string cmd = "command -v " + name + " >/dev/null 2>&1";
    return system(cmd.c_str()) == 0;
}

static std::vector<std::string> parse_uuid_list(const std::string &dconf_list_raw);
static std::string find_or_create_profile(const std::string &profile_name, const std::string &dconf_path);

void gnome::loadProfile(int argc, char **argv)
{
    bool inside_run = false;
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--chess-run")
        {
            inside_run = true;
            break;
        }
    }

    if (inside_run)
    {
        return;
    }

    if (!command_exists("gnome-terminal") || !command_exists("dconf"))
    {
        return;
    }

    const std::string CHESS_PROFILE_NAME = "Chess";
    const std::string CHESS_DCONF_PATH = "src/utils/tui/chess.dconf";

    std::string uuid = find_or_create_profile(CHESS_PROFILE_NAME, CHESS_DCONF_PATH);

    if (uuid.empty())
    {
        std::cerr << "Warning: Failed to create or locate the Chess profile. Running in standard mode.\n";
        return;
    }

    std::string exe = argv[0];
    std::ostringstream launch;
    launch << "gnome-terminal --window-with-profile=\"" << CHESS_PROFILE_NAME
           << "\" -- "
           << "'" << exe << "' --chess-run &";

    exec_nowait(launch.str());
    exit(0);
}

static std::vector<std::string> parse_uuid_list(const std::string &dconf_list_raw)
{
    std::vector<std::string> uuids;
    std::regex re("([0-9a-fA-F\\-]{36})");
    std::smatch m;
    std::string s = dconf_list_raw;
    auto it = s.cbegin();
    while (std::regex_search(it, s.cend(), m, re))
    {
        uuids.push_back(m[1].str());
        it = m.suffix().first;
    }
    return uuids;
}

static std::string find_or_create_profile(const std::string &profile_name, const std::string &dconf_path)
{
    std::string list_raw = exec_capture("dconf read /org/gnome/terminal/legacy/profiles:/list 2>/dev/null");
    auto uuids = parse_uuid_list(list_raw);

    for (const auto &u : uuids)
    {
        std::string cmd = "dconf read /org/gnome/terminal/legacy/profiles:/:" + u + "/visible-name 2>/dev/null";
        std::string got = exec_capture(cmd);
        if (!got.empty())
        {
            if (got.size() >= 2 && got.front() == '\'' && got.back() == '\'')
                got = got.substr(1, got.size() - 2);
            if (got == profile_name)
                return u;
        }
    }

    if (!std::filesystem::exists(dconf_path))
    {
        std::cerr << "Error: dconf file not found at: " << dconf_path << "\n";
        return "";
    }

    std::string newuuid = exec_capture("uuidgen");
    if (newuuid.empty())
    {
        std::cerr << "Error: uuidgen failed\n";
        return "";
    }

    std::string load_cmd = "dconf load /org/gnome/terminal/legacy/profiles:/:" + newuuid + "/ < '" + dconf_path + "' 2>/dev/null";
    exec_nowait(load_cmd);

    std::vector<std::string> final_uuids = uuids;
    final_uuids.push_back(newuuid);

    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < final_uuids.size(); ++i)
    {
        oss << "'" << final_uuids[i] << "'";
        if (i + 1 < final_uuids.size())
            oss << ", ";
    }
    oss << "]";

    std::string write_list_cmd = "dconf write /org/gnome/terminal/legacy/profiles:/list \"" + oss.str() + "\" 2>/dev/null";
    exec_nowait(write_list_cmd);

    std::string setname_cmd = "dconf write /org/gnome/terminal/legacy/profiles:/:" + newuuid + "/visible-name \"'" + profile_name + "'\" 2>/dev/null";
    exec_nowait(setname_cmd);

    return newuuid;
}
