//
// Created by linayang on 3/19/24.
//

#ifndef COMP_4985_PROTOCOL_TESTER_BUILDINGCLIENTS_H
#define COMP_4985_PROTOCOL_TESTER_BUILDINGCLIENTS_H
int create_client_max(int argc, char *argv[]);
int check_client_chat_message(int argc, char *argv[]);
int check_format(int argc, char *argv[]);
int check_username_command(int argc, char *argv[]);
int check_userlist_command(int argc, char *argv[]);
int check_whisper_command(int argc, char *argv[]);
int check_help_command(int argc, char *argv[]);
int check_username_already_exists(int argc, char *argv[]);
int check_version_max(int argc, char *argv[]);
int check_version_min(int argc, char *argv[]);
int check_content_size_max(int argc, char *argv[]);
int check_content_size_min(int argc, char *argv[]);
int check_version_num(int argc, char *argv[]);
#endif //COMP_4985_PROTOCOL_TESTER_BUILDINGCLIENTS_H
