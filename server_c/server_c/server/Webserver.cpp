#include "server/h/Webserver.h"



int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


string Webserver::get_client_id_map(int key)
{
    if (client_id_map.find(key) != client_id_map.end()) {
        return client_id_map[key];
    }
    else {
        return "none";
    }
}

string Webserver::get_client_id_method_map(int key)
{
    if (client_id_method_map.find(key) != client_id_method_map.end()) {
        return client_id_method_map[key];
    }
    else {
        return "none";
    }
}

void Webserver::disconnect(int epoll_fd, int client_fd) {
    std::cout << "Client disconnect: " << client_fd<< " "<< this->get_client_id_method_map(client_fd) << std::endl;
    this->client_id_map.erase(client_fd);
    this->client_id_len_map.erase(client_fd);

    if (this->get_client_id_method_map(client_fd) == "GET") {
        this->client_id_params_map_map.erase(client_fd);
    }

    this->client_id_method_map.erase(client_fd);
    this->client_id_url_map.erase(client_fd);
    this->client_id_params_map.erase(client_fd);
    this->client_id_HTTP_map.erase(client_fd);

    close(client_fd);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
}


void Webserver::cycle()
{   

    // 创建 epoll 实例，用于让server_fd放进epoll里面
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        std::cerr << "Failed to create epoll file descriptor\n";
        close(server_fd);
    }
    // 将监听套接字加入 epoll 实例中
    struct epoll_event event;
    event.events = EPOLLIN; // 监听输入（连接）事件
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
    {
        std::cerr << "Failed to add server socket to epoll\n";
        close(server_fd);
        close(epoll_fd);
    }

    std::vector<epoll_event> events(MAX_EVENTS);

    while (true) {
        int event_count = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, -1);
        if (event_count == -1)
        {
            std::cerr << "Error during epoll_wait\n";
            break;
        }

        for (int i = 0; i < event_count; ++i)
        {
            if (events[i].data.fd == server_fd)
            {
                int client_fd = this->wait_request();

                // 将新连接的客户端套接字加入 epoll 实例，监听 EPOLLIN 事件
                struct epoll_event client_event;
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event);

            }
            else {
                int client_fd = events[i].data.fd;

                //std::cout << "client_fd: " << client_fd << std::endl;
                // 当 events[i].events 包含 EPOLLIN 时，执行此代码块
                if (events[i].events & EPOLLIN)
                {
                    char buffer[1024];
                    ssize_t count = recv(client_fd, buffer, sizeof(buffer), 0);
                    if (count == -1)
                    {
                        std::cerr << "Error reading from client\n";
                        close(client_fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                    }
                    else if (count == 0)
                    {
                        disconnect(epoll_fd, client_fd);
                        
                    }
                    else
                    {
                        buffer[count] = '\0';
                        std::string request(buffer);
                        std::cout << "Received: " << buffer << std::endl;


                        std::istringstream request_stream(request);

                        
                        std::string line;
                        while (std::getline(request_stream, line)) {
                            //std::cout << "get:" << line << std::endl;

                            // 处理url和url后面的参数
                            if (this->get_client_id_method_map(client_fd) == "none") {
                                string http_method = line.substr(0, 3);
                                int pos_ = 4;

                                this->client_id_params_map[client_fd] = "";

                                if (http_method == "POS") {
                                    pos_ = 5;
                                    this->client_id_method_map[client_fd] = "POST";
                                }
                                else if (http_method == "GET") {
                                    this->client_id_method_map[client_fd] = "GET";
                                }
                                string http_url = line.substr(pos_);

                                if (http_url.find("HTTP/") != std::string::npos) {
                                    this->client_id_HTTP_map[client_fd] = true;
                                }
                                else {
                                    this->client_id_HTTP_map[client_fd] = false;
                                }
                                this->client_id_params_map[client_fd] = this->client_id_params_map[client_fd] + http_url;
                                continue;
                            }
                            // 处理url和url后面的参数2
                            if (! this->client_id_HTTP_map[client_fd]) {
                                if (line.find("HTTP/") != std::string::npos) {
                                    this->client_id_HTTP_map[client_fd] = true;
                                }
                                this->client_id_params_map[client_fd] = this->client_id_params_map[client_fd] + line;
                                continue;
                            }
                            else {
                                // 处理url和url后面的参数3
                                if (client_id_url_map.find(client_fd) == client_id_url_map.end()) {
                                    string all_params = this->client_id_params_map[client_fd];
                                    size_t konbai_ = all_params.find(" ");
                                    all_params = all_params.substr(0, konbai_);
                                    size_t wenhao_pos = all_params.find("?");
                                    string url;
                                    string params = "";

                                    if (wenhao_pos != std::string::npos) {
                                        url = all_params.substr(0, wenhao_pos);
                                        params = all_params.substr(wenhao_pos + 1);
                                    }
                                    else {
                                        url = all_params;
                                    }
                                    this->client_id_url_map[client_fd] = url;
                                    this->client_id_params_map[client_fd] = params;
                                }
                            }
                                
                    /*        std::cout << "url:" << this->client_id_url_map[client_fd] << std::endl;
                            std::cout << "method:" << this->client_id_method_map[client_fd] << std::endl;*/

                            //if (this->get_client_id_method_map(client_fd) == "POST") {
                                if (!line.empty() && line[0] == '\r') {
                                    if (this->get_client_id_map(client_fd) == "none") {
                                        this->client_id_map[client_fd] = "";
                                    }
                                }
                                else {
                                    if (this->get_client_id_map(client_fd) != "none") {
                                        this->client_id_map[client_fd] = this->client_id_map[client_fd] + line;
                                    }
                                }

                                if (line.find("Content-Length: ") != std::string::npos) {
                                    std::string result = line.substr(16);
                                    int content_length;
                                    std::stringstream ss(result);
                                    ss >> content_length;
                                    client_id_len_map[client_fd] = content_length;
                                }
                            //}
                        }



                        int http_not_complete = true;
                        if (this->get_client_id_map(client_fd) != "none") {
                            string data_s = client_id_map[client_fd];

                            if (data_s.size() == client_id_len_map[client_fd]) {
                                http_not_complete = false;
                            }
                        }

                        if (http_not_complete) {
                            this->send_data_pending(client_fd);
                            continue;
                        }

                        if (this->get_client_id_method_map(client_fd) == "POST") {
                            this->send_data_post(client_fd);
                        }
                        else if (this->get_client_id_method_map(client_fd) == "GET") {
                            //std::cout << "params:" << this->client_id_params_map[client_fd] << std::endl;

                            vector<std::string> string_split = str_tools.splitString(this->client_id_params_map[client_fd], '&');
                            size_t denghao;
                            string key, value;

                            std::map<string, string> params_map_map;
                            for (const auto& a : string_split) {
                                denghao = a.find("=");
                                if (denghao != std::string::npos) {
                                    key = a.substr(0, denghao);
                                    value = a.substr(denghao+1);
                                    params_map_map[key] = value;

                                }
                            }


                            this->client_id_params_map_map[client_fd] = params_map_map;

                            this->send_data_get(client_fd);
                        }

                        disconnect(epoll_fd, client_fd);

                        /*if (this->get_client_id_map(client_fd) != "none") {
                            
                            std::string result = std::string("thisthisthis: ") + this->client_id_map[client_fd];
                            std::cout << result << std::endl;
                        }*/
       

                        
                    }
                    
                }

            }
        }
    }
   
}


void Webserver::send_data_pending(int client_fd) {
    std::string http_response =
        "HTTP/1.1 102 Processing\r\n"
        "Content-Length: 0\r\n"
        "\r\n";

    int send_re = send(client_fd, http_response.c_str(), http_response.size(), 0);
    if (send_re == -1) {
        perror("send failed");

    }

}


void Webserver::send_data_get(int client_fd) {
    std::string http_response;

    std::string json_str = json_tools.map_to_json(client_id_params_map_map[client_fd]);
    http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n";

    http_response = http_response + "Content-Length: " + std::to_string(json_str.size()) + "\r\n"
        "\r\n" + json_str;
 


    int send_re = send(client_fd, http_response.c_str(), http_response.size(), 0);
    if (send_re == -1) {
        perror("send failed");

    }

    

}

void Webserver::send_data_post(int client_fd) {

    std::string http_response;
    string data_s = client_id_map[client_fd];

    if (data_s.size() == client_id_len_map[client_fd]) {
        http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n";

        nlohmann::json data = json_tools.string_to_json(data_s);

        http_response = http_response + "Content-Length: " + std::to_string(data["a"].get<std::string>().size()) + "\r\n"
            "\r\n" + data["a"].get<std::string>();
    }
    
    int send_re = send(client_fd, http_response.c_str(), http_response.size(), 0);
    if (send_re == -1) {
        perror("send failed");
    }
    
}

void Webserver::init_server() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(server_fd >= 0);  // 确保套接字创建成功
    printf("server_fd: %d\n", server_fd);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);  
    address.sin_port = htons(port);  

    int bind_re = ::bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    assert(bind_re != -1); 
    printf("bind_re: %d\n", bind_re);

    int listen_re = listen(server_fd, 5);
    assert(listen_re >= 0);  
    printf("listen_re: %d\n", listen_re);

    printf("Server is listening on port: %d\n", port);


    // 给自动分配请求用的
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

}

int Webserver::wait_request() {
    struct sockaddr_in addrClient;
    socklen_t len = sizeof(addrClient);  

    int sock_connect = accept(server_fd, (struct sockaddr*)&addrClient, &len);
    if (sock_connect == -1) {
        perror("Accept failed");  
        exit(1);
    }

    set_nonblocking(sock_connect);

    //printf("Accept client IP: [%s]\n", inet_ntoa(addrClient.sin_addr));
    return sock_connect;
}





void Webserver::close_() {
    ::close(server_fd);
}
