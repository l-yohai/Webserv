/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 16:14:31 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/09 18:59:29 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

#include <vector>

class ServerManager
{
private:
        // 한 바퀴 돌면서 server block 밖의 설정파일을 저장한다.
        // find(server block) and setting -> 만약 globalCOnfig에 있는게 없다면 ADD
    std::vector<ServerConifg *> _configs;
    std::vector<Server *> _servers;
public:
    /* Constructor */
    //TODO: 생성자 인자를 넣는 방식이 최선인지 고민해보기.
    ServerManager();
    ServerManager(char* config_path="default config_path");
    ServerManager(const ServerManager& other);

    /* Destructor */
    virtual ~ServerManager();

    /* Overload */
    ServerManager& operator=(const ServerManager& rhs);

    /* Getter */
    const std::vector<ServerConfig *> getConfigs();
    const std::Vector<Server *> getServers();


    /* Setter */
    bool setConfigs(char *path);

    /* Exception */
    /* Util */

    void initServers();
    void makeServer(struct s_config);
    bool runServers();
    void exitServers();
};

#endif