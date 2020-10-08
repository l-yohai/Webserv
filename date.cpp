/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   date.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yohlee <yohlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/08 14:55:42 by yohlee            #+#    #+#             */
/*   Updated: 2020/10/08 15:55:07 by yohlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/time.h>
#include <unistd.h>

int	main()
{
	struct tm 		time;
	struct timeval 	tv;
	char 			buf[128];
	std::string 	date;

	gettimeofday(&tv, NULL);
	strptime(std::to_string(tv.tv_sec).c_str(), "%s", &time);
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &time);

	std::string msg;

	msg += "Date: " + static_cast<std::string>(buf);

	std::cout << msg << std::endl << std::endl;
}