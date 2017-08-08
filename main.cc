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

void readTokenFile(std::string tokenFilePath);

struct tokeninfo{
	std::string addr;
	std::string port;
	std::string token1;
	std::string token2;
	std::string account;
	std::string password;
} fileout;
using json = nlohmann::json;
using aios_ptr = std::shared_ptr<asio::io_service>;

int main() {
    std::cout << "Starting bot...\n\n";

    /*/
     * Read token from token file.
    /*/

    std::string token, token2, token3;
    if(boost::filesystem::exists("token.dat")){
        readTokenFile("token.dat");
        //token3 = readTokenFile("token3.dat");
    } else {
        std::cerr << "CRITICAL: Missing token file.\n";
        exit(1);
    }

    aios_ptr aios = std::make_shared<asio::io_service>();

    discordpp::Bot bot(
            aios,
            fileout.token1,
            std::make_shared<discordpp::RestCurlPPModule>(aios, fileout.token1),
            std::make_shared<discordpp::WebsocketWebsocketPPModule>(aios, fileout.token1)
    );

    discordpp::Bot bot2(
            aios,
            fileout.token2,
            std::make_shared<discordpp::RestCurlPPModule>(aios, fileout.token2),
            std::make_shared<discordpp::WebsocketWebsocketPPModule>(aios, fileout.token2)
    );


    //tcp_server tcpbot(*aios, 13);

    connection tcpclient(*aios, fileout.addr, fileout.port, fileout.account, fileout.password);

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
            //tcpclient.write(content);
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
            return;
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
	tcpclient.add_read_handler([&tcpclient, &bot, &bot2/*, &bot3*/](const std::string& msg) {
		std::istringstream iss(msg);
		std::string command, user, msg2;
			return;
		iss >> command >> user >> msg2; 
		int botno = 0;
		if(command != "announce")
			return;

        json guild;
        for(json& guildptr : bot.guilds_){
                if(guildptr["name"] == "testeserver"){
                    guild = guildptr;
                    break;
                }
        }
            bot.call(
                    "/channels/"+guild["id"].get<std::string>()+"/webhooks",
                    {}
                    ,
                    "GET",[command, user, msg2, &bot2](discordpp::Bot* bot, json msg1){
                    json hook;
                    for(json& webptr : msg1){
                        if(webptr["name"] == "ABC"){
                            hook = webptr;
                            break;
                        }
                    }
                    /*
                        std::string content = msg["content"].get<std::string>()+"\n";
                    

                        std::string user = content.substr(0, content.find(" "));

                        content = content.substr(content.find(" ")+1);
                        std::string url = content.substr(0, content.find(" "));

                        std::string msgg = content.substr(content.find(" ")+1); */


                    int botno=0;
            try
            {
              bot->call(
                        "/webhooks/" + hook["id"].get<std::string>() + "/"+hook["token"].get<std::string>(),
                        {{"content", msg2},{"username", user},{"avatar_url", "http://s8.picofile.com/file/8270044084/WOWRANKING_epsilonwow_ALL_GM_CATA_4_3_4.png"}},
                        "POST"
                );
            }
            catch(discordpp::ratelimit a)
            {
                botno++;
            } 
            try
            {
            	if(botno == 1){
            	bot2.call(
                        "/webhooks/" + hook["id"].get<std::string>() + "/"+hook["token"].get<std::string>(),
                        {{"content", msg2},{"username", user},{"avatar_url", "http://s8.picofile.com/file/8270044084/WOWRANKING_epsilonwow_ALL_GM_CATA_4_3_4.png"}},
                        "POST"
                );
            }

            }/*
            catch(discordpp::ratelimit a)
            {
                botno++;
            } 
            try
            {
                if(botno == 2)
                {
                	    bot3.call(
                        "/channels/" + guild["id"].get<std::string>() + "/messages",
                        {{"content", content}},
                        "POST"
                        );
                                    
                }
            } */
            catch(discordpp::ratelimit a)
            {
                std::cout << "ratelimit reached\n";
            } 
			}); 
		});   

		tcpclient.add_read_handler([&tcpclient, &bot, &bot2/*, &bot3*/](const std::string& msg) {
			/*std::istringstream iss(msg);
			std::string from, type, to, msg;
			
			iss >> from >> type >> to >> msg; */
			//return;
			int botno = 0;
			std::string content = msg+"\n";
            json guild;
            for(json& guildptr : bot.guilds_){
                    if(guildptr["name"] == "testeserver"){
                        guild = guildptr;
                        break;
                    }
            }

            try
            {
                        bot.call(
                        "/channels/" + guild["id"].get<std::string>() + "/messages",
                        {{"content", content}},
                        "POST"
            );
            }
            catch(discordpp::ratelimit a)
            {
                botno++;
            } 
            try
            {
                if(botno == 1)
                {
                	    bot2.call(
                        "/channels/" + guild["id"].get<std::string>() + "/messages",
                        {{"content", content}},
                        "POST"
                        );
                                    
                }
            }/*
            catch(discordpp::ratelimit a)
            {
                botno++;
            } 
            try
            {
                if(botno == 2)
                {
                	    bot3.call(
                        "/channels/" + guild["id"].get<std::string>() + "/messages",
                        {{"content", content}},
                        "POST"
                        );
                                    
                }
            } */
            catch(discordpp::ratelimit a)
            {
                std::cout << "ratelimit reached\n";
            } 
		});

		bot.addHandler("MESSAGE_CREATE", [&tcpclient](discordpp::Bot* bot, json msg){

			if(msg["author"]["id"] == bot->me_["id"]){
                return;
            }

			json guild;
            for(json& guildptr : bot->guilds_){
                    if(guildptr["name"] == "testeserver"){
                        guild = guildptr;
                       
                    }
                    
            }
            if(guild["id"] != msg["channel_id"])
                        return;
                  

            //tcpclient.write("announce "+msg["author"]["username"].get<std::string>()+": "+msg["content"].get<std::string>());
            tcpclient.write(msg["content"].get<std::string>());

		});


    //out.send("hello"); 


    

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

void readTokenFile(std::string tokenFilePath){
    std::ifstream tokenFile;
    tokenFile.open(tokenFilePath);

    std::string token;

    if (tokenFile.is_open()) {
        while(std::getline(tokenFile, token))
        {
        	if(token.find("Token1: ")!=std::string::npos)
        		fileout.token1 = token.substr(strlen("Token1: "), std::string::npos);
        	else if(token.find("Token2: ")!=std::string::npos)
        		fileout.token2 = token.substr(strlen("Token2: "), std::string::npos);
        	else if(token.find("Address: ")!=std::string::npos)
        		fileout.addr = token.substr(strlen("Address: "), std::string::npos);
        	else if(token.find("Port: ")!=std::string::npos)
        		fileout.port = token.substr(strlen("Port: "), std::string::npos);
        	else if(token.find("Account: ")!=std::string::npos)
        		fileout.account = token.substr(strlen("Account: "), std::string::npos);
        	else if(token.find("Password: ")!=std::string::npos)
        		fileout.password = token.substr(strlen("Password: "), std::string::npos);        		        	

        }
    } else {
        std::cerr << "CRITICAL: There is no such file as " + tokenFilePath + "! Copy the example login.dat to make one.\n";
        exit(1);
    }
    tokenFile.close();
    std::cout << "Retrieved token.\n\n";

}