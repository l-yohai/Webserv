/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/09 16:19:44 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/09 16:21:43 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef Server_HPP
# define Server_HPP

# include "ServerManager.hpp"

class Server
{
private:
	struct s_config _config;
	int _server_socket;
	std::vector<int> _client_sockets;
public:
	/* Constructor */
	Server();
	Server(struct s_config server_config);
	Server(const Server& other);

	/* Destructor */
	virtual ~Server();

	/* Overload */
	Server& operator=(const Server& rhs);

	/* Getter */
	/* Setter */
	/* Exception */
	/* Util */

	bool init();

}

#endif