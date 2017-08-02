#ifndef EXAMPLE_BOT_REST_CURLPP_HH
#define EXAMPLE_BOT_REST_CURLPP_HH

#include <chrono>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#define STDC_HEADERS 1

#include <discordpp/restmodule.hh>

namespace discordpp{
    using json = nlohmann::json;

    class RestCurlPPModule : public RestModule{
    public:
        RestCurlPPModule(std::shared_ptr<asio::io_service> asio_ios, const std::string &token):
                RestModule(asio_ios, token){}
        json call(std::string target, std::string token, json body, std::string requestType){
            try
            {
                std::stringstream outstream;

                cURLpp::Cleanup clean;
                //curlpp::Cleanup clean;
                curlpp::Easy request;
                curlpp::options::WriteStream ws(&outstream);
                request.setOpt(ws);
                request.setOpt<curlpp::options::Url>("https://discordapp.com/api" + target);
                request.setOpt(curlpp::options::Verbose(false));

                if(!requestType.empty()) {
                    request.setOpt(curlpp::options::CustomRequest(requestType));
                }

                std::list<std::string> header;
                header.push_back("Content-Type: application/json");
                if(token != "") {
                    header.push_back(std::string("Authorization: ") + token);\
                }

                request.setOpt(curlpp::options::HttpHeader(header));

                if(!body.empty()) {
                    //std::cout << attachJSON.dump() << std::endl;
                    request.setOpt(curlpp::options::PostFields(body.dump()));
                    request.setOpt(curlpp::options::PostFieldSize(body.dump().length()));
                }

                request.perform();
                  //  std::cout << outstream.str() << "\n";
                json returned = json::parse(outstream.str());

                try {
                    //std::cout << returned.dump() << std::endl;

                    std::string message = returned["message"].get<std::string>();
                    //std::cout << returned.dump() << std::endl;
                    if(message == "You are being rate limited."){
                        throw (ratelimit){returned["retry_after"].get<int>()};
                        //std::this_thread::sleep_for(std::chrono::milliseconds(returned["retry_after"].get<int>()));
                    }else if(message != "") {
                        std::cout << "Discord API sent a message: \"" << message << "\"" << std::endl;
                    }
                } catch ( std::out_of_range & e) {

                } catch ( std::domain_error & e) {

                }
                
                return returned;
            }
            catch ( curlpp::LogicError & e ) {
                std::cout << "logic " << e.what() << std::endl;
            }
            catch ( curlpp::RuntimeError & e ) {
                std::cout << "runtime " << e.what() << std::endl;
            }
            catch ( std::invalid_argument & e ) {

            }

            return {};
        }
    };
}

#endif //EXAMPLE_BOT_REST_CURLPP_HH
