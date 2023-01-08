CLIENT = main.cpp ./source/Data.cpp ./source/Ftp.cpp ./source/Ftps.cpp ./source/Http.cpp ./source/Https.cpp ./source/Manager.cpp ./source/Parser.cpp
CLIENT_O = $(CLIENT:.cpp=.o)
all: client

client: $(CLIENT_O)
	g++ $(CLIENT)  -o DownloadManager -l ssl -l crypto -l pthread
	./DownloadManager

clean:
	rm -f client $(CLIENT_O) DownloadManager