#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
namespace asio = boost::asio;



#include <discordpp/bot.hh>
#include <discordpp/rest-curlpp.hh>
#include <discordpp/websocket-websocketpp.hh>
#include <fstream>
#include <discordpp/sendp.hh>
#include <discordpp/tcpserver.hh>

#include <boost/thread.hpp>
#include <discordpp/test.hh>
//#include <lib/nlohmannjson/src/json.hpp>
//#include <nlohmann/json.hpp>

std::string readTokenFile(std::string tokenFilePath);

using json = nlohmann::json;
using aios_ptr = std::shared_ptr<asio::io_service>;

int main() {
    std::cout << "Starting bot...\n\n";

    /*/
     * Read token from token file.
     * Tokens are required to communicate with Discord, and hardcoding tokens is a bad idea.
     * If your bot is open source, make sure it's ignore by git in your .gitignore file.
    /*/

    std::string token, token2;
    if(boost::filesystem::exists("token.dat") && boost::filesystem::exists("token2.dat")){
        token = readTokenFile("token.dat");
        token2 = readTokenFile("token2.dat");
    } else {
        std::cerr << "CRITICAL: There is no valid way for Discord++ to obtain a token! Copy the example login.dat or token.dat to make one.\n";
        exit(1);
    }

    aios_ptr aios = std::make_shared<asio::io_service>();

    discordpp::Bot bot(
            aios,
            token,
            std::make_shared<discordpp::RestCurlPPModule>(aios, token),
            std::make_shared<discordpp::WebsocketWebsocketPPModule>(aios, token)
    );

    discordpp::Bot bot2(
            aios,
            token2,
            std::make_shared<discordpp::RestCurlPPModule>(aios, token2),
            std::make_shared<discordpp::WebsocketWebsocketPPModule>(aios, token2)
    );

    tcp_server tcpbot(*aios, 13);

    connection tcpclient(*aios, "127.0.0.1", "13");

    //discordpp::sendpack out("localhost",13);
/*
    server_thread();
    client_thread(); */


    bot.addHandler("MESSAGE_CREATE", [&tcpclient](discordpp::Bot* bot, json msg){
        //std::cout << bot->me_.dump(4) << '\n';
        //std::cout << msg.dump() << '\n';
        bool mentioned = false;
        for(auto mention : msg["mentions"]) {
            if(mention["id"] == bot->me_["id"]){
                mentioned = true;
                break;
            }
        }
        if(mentioned){
            std::string mentioncode = "<@" + bot->me_["id"].get<std::string>() + ">";
            std::string content = msg["content"];
            while(content.find(mentioncode + ' ') != std::string::npos) {
                content = content.substr(0, content.find(mentioncode + ' ')) + content.substr(content.find(mentioncode + ' ') + (mentioncode + ' ').size());
            }
            while(content.find(mentioncode) != std::string::npos) {
                content = content.substr(0, content.find(mentioncode)) + content.substr(content.find(mentioncode) + mentioncode.size());
            }
            bot->call(
                    "/channels/" + msg["channel_id"].get<std::string>() + "/messages",
                    {{"content", content}},
                    "POST"
            );
            tcpclient.write(content);
            bot->send(3, {
                    {"game", {
                            {"name", "with " + msg["author"]["username"].get<std::string>()}
                    }},
                    {"status", "online"},
                    {"afk", false},
                    {"since", "null"}
            });
        }
    });

    bot.addHandler("MESSAGE_CREATE", [](discordpp::Bot* bot, json msg){
            
            json guild;
            for(json& guildptr : bot->guilds_){
                    if(guildptr["name"] == "osu!pt"){
                        guild = guildptr;
                       
                    }
                    if(guildptr["id"] == msg["channel_id"])
                        return;
            }

            std::cout << guild["id"].get<std::string>() + "\n";


            bot->call(
                    "/channels/"+guild["id"].get<std::string>()+"/webhooks",
                    {}
                    ,
                    "GET",[msg](discordpp::Bot* bot, json msg1){
                    json hook;
                    for(json& webptr : msg1){
                        if(webptr["name"] == "ABC"){
                            hook = webptr;
                            break;
                        }
                    }
                        std::string content = msg["content"].get<std::string>()+"\n";
                    

                        std::string user = content.substr(0, content.find(" "));

                        content = content.substr(content.find(" ")+1);
                        std::string url = content.substr(0, content.find(" "));

                        std::string msgg = content.substr(content.find(" ")+1);

                bot->call(
                        "/webhooks/" + hook["id"].get<std::string>() + "/"+hook["token"].get<std::string>(),
                        {{"content", msgg},{"username", user},{"avatar_url", url}},
                        "POST"
                );
               
        });
    });




    //out.send("hello"); 

    
    tcpbot.addHandler("MESSAGE_CREATE", [&bot, &bot2](tcp_server* bot, std::string msg){
            std::string content = msg+"\n";
            json guild;
            for(json& guildptr : bot.guilds_){
                    if(guildptr["name"] == "testeserver"){
                        guild = guildptr;
                        break;
                    }
            }
            bool rateLimit = false;
            try
            {
                bot.call(
                        "/channels/" + guild["id"].get<std::string>() + "/messages",
                        {{"content", content}},
                        "POST"
                );
                bot.send(3, {
                        {"game", {
                            {"name", "with himself"}
                        }},
                        {"status", "online"},
                        {"afk", false},
                        {"since", "null"}
                }); 
            }
            catch(discordpp::ratelimit a)
            {
                rateLimit = true;
            } 
            try
            {
                if(rateLimit == true)
                {
                    bot2.call(
                        "/channels/" + guild["id"].get<std::string>() + "/messages",
                        {{"content", content}},
                        "POST"
                    );
                    bot2.send(3, {
                        {"game", {
                            {"name", "with himself"}
                        }},
                        {"status", "online"},
                        {"afk", false},
                        {"since", "null"}
                    });                 
                }
            }
            catch(discordpp::ratelimit a)
            {
                std::cout << "ratelimit reached\n";
            } 

    });

    tcpbot.addHandler("MESSAGE_CREATE1", [&bot, &bot2](tcp_server* bott, std::string msg){
            std::string content = msg+"\n";
            json guild;
            for(json& guildptr : bot.guilds_){
                    if(guildptr["name"] == "testeserver"){
                        guild = guildptr;
                        break;
                    }
            }
            std::cout << guild["id"].get<std::string>() + "\n";
            bot.call(
                    "/channels/"+guild["id"].get<std::string>()+"/webhooks",
                    {}
                    ,
                    "GET",[msg](discordpp::Bot* bot, json msg1){
                    json hook;
                    for(json& webptr : msg1){
                        if(webptr["name"] == "ABC"){
                            hook = webptr;
                            break;
                        }
                    }
                        std::string content = msg+"\n";

                bot->call(
                        "/webhooks/" + hook["id"].get<std::string>() + "/"+hook["token"].get<std::string>(),
                        {{"content", content}},
                        "POST"
                );
               
        });
            

    });

    bot.addHandler("PRESENCE_UPDATE", [](discordpp::Bot*, json) {
        // ignore
    });
    bot.addHandler("TYPING_START", [](discordpp::Bot*, json) {
        // ignore
    });

    bot2.addHandler("PRESENCE_UPDATE", [](discordpp::Bot*, json) {
        // ignore
    });
    bot2.addHandler("TYPING_START", [](discordpp::Bot*, json) {
        // ignore
    });

    aios->run();

    return 0;
}

std::string readTokenFile(std::string tokenFilePath){
    std::ifstream tokenFile;
    tokenFile.open(tokenFilePath);

    std::string token;

    if (tokenFile.is_open()) {
        std::getline(tokenFile, token);
    } else {
        std::cerr << "CRITICAL: There is no such file as " + tokenFilePath + "! Copy the example login.dat to make one.\n";
        exit(1);
    }
    tokenFile.close();
    std::cout << "Retrieved token.\n\n";

    return token;
}