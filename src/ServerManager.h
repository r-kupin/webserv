/******************************************************************************/
/*                                                                            */
/*                                                         :::      ::::::::  */
/*    ServerManager.h                                    :+:      :+:    :+:  */
/*                                                     +:+ +:+         +:+    */
/*    By: rokupin <rokupin@student.42.fr>            +#+  +:+       +#+       */
/*                                                 +#+#+#+#+#+   +#+          */
/*    Created: 2023/04/11 12:15:17 by rokupin           #+#    #+#            */
/*                                                     ###   ########.fr      */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERV_LIB_SERVERMANAGER_H
#define WEBSERV_LIB_SERVERMANAGER_H


class ServerManager {
public:
    ServerManager();

    ServerManager(const ServerManager &);

    ServerManager &operator=(const ServerManager &);

    ~ServerManager();

private:
};


#endif //WEBSERV_LIB_SERVERMANAGER_H