#include "TCPClient.h"

TCPClient::TCPClient() {
    sock = -1;
	port = 0;
	address = "";
}

TCPClient::TCPClient(string address, int port) {
    try {
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1) {
  			cout << "Could not create socket" << endl;
		}
      	if(inet_addr(address.c_str()) == -1) {
    		struct hostent *he;
    		struct in_addr **addr_list;
    		if ((he = gethostbyname(address.c_str())) == NULL) {
                herror("gethostbyname");
                cout << "Failed to resolve hostname\n";
    		}
       	    addr_list = (struct in_addr **) he->h_addr_list;
    		for(int i = 0; addr_list[i] != NULL; i++) {
                server.sin_addr = *addr_list[i];
                break;
    		}
      	}
      	else {
    		server.sin_addr.s_addr = inet_addr(address.c_str());
      	}
      	server.sin_family = AF_INET;
      	server.sin_port = htons(port);
      	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            perror("connect failed. Error");
      	}
    } catch (const exception & ex) {
        cerr << "ERROR: Impossible create TCP client!" << endl;
        cerr << "Stacktrace:" << ex.what() << endl;
    }
}

bool TCPClient::sendData(string data) {
	if(sock != -1) {
		if (send(sock, data.c_str(), strlen( data.c_str()), 0) < 0) {
			cout << "Send failed : " << data << endl;
			return false;
		}
	}
	else {
        return false;
    }
	return true;
}

string TCPClient::receiveData(int size) {
  	char buffer[size];
	memset(&buffer[0], 0, sizeof(buffer));

  	string reply;
	if(recv(sock , buffer , size, 0) < 0) {
    	cout << "receive failed!" << endl;
		return nullptr;
  	}
	buffer[size-1]='\0';
  	reply = buffer;
  	return reply;
}

string TCPClient::read() {
  	char buffer[1] = {};
  	string reply;
  	while (buffer[0] != ';') {
    	if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
			cout << "receive failed!" << endl;
            return nullptr;
    	}
        if (buffer[0] != ';') {
            reply += buffer[0];
        }
	}
	return reply;
}

void TCPClient::exit() {
    close(sock);
}