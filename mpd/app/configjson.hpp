
// Copyright (c) 2023. ByteDance Inc. All rights reserved.

#pragma once
#include "nlohmann/json.hpp"
using json = nlohmann::json;
struct UpNodeConfig
{
    std::string IP="127.0.0.1";
    uint16_t port=41111;
    std::string selfpeerID="0001020304050607080910111213141516171800"; //20 HEX number, 40 chars in total
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UpNodeConfig,IP,port,selfpeerID)

struct DownNodeConfig
{
    DownNodeConfig()
    {
        UpNodeConfig up1;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up2;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up3;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up4;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up5;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up6;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up7;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up8;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up9;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up10;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up11;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up12;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up13;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up14;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up15;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up16;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up17;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up18;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up19;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";


        UpNodeConfig up20;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
/*
        
        UpNodeConfig up21;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up22;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up23;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up24;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up25;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up26;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up27;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up28;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up29;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up30;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up31;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up32;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up33;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up34;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up35;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up36;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up37;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up38;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up39;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up40;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up41;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up42;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up43;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up44;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up45;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up46;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up47;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up48;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up49;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up50;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up51;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up52;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up53;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up54;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up55;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up56;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up57;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up58;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up59;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up60;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up61;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up62;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up63;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up64;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up65;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up66;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up67;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up68;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up69;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up70;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up71;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up72;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up73;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up74;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up75;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up76;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up77;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up78;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up79;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up80;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up81;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up82;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up83;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up84;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up85;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up86;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up87;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up88;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up89;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up90;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up91;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up92;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up93;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up94;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";

        UpNodeConfig up95;
        up1.IP = "10.0.0.2";
        up1.port = 41111;
        up1.selfpeerID = "0001020304050607080910111213141516171801";

        UpNodeConfig up96;
        up2.IP = "10.0.0.3";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171802";
        
        UpNodeConfig up97;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up98;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        
        UpNodeConfig up99;
        up2.IP = "10.0.0.4";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171803";

        UpNodeConfig up100;
        up2.IP = "10.0.0.5";
        up2.port = 41111;
        up2.selfpeerID = "0001020304050607080910111213141516171804";
        */

        upnodes.emplace_back(up1);
        upnodes.emplace_back(up2);
        upnodes.emplace_back(up3);
        upnodes.emplace_back(up4);
        upnodes.emplace_back(up5);
        upnodes.emplace_back(up6);
        upnodes.emplace_back(up7);
        upnodes.emplace_back(up8);
        upnodes.emplace_back(up9);
        upnodes.emplace_back(up10);
        upnodes.emplace_back(up11);
        upnodes.emplace_back(up12);
        upnodes.emplace_back(up13);
        upnodes.emplace_back(up14);
        upnodes.emplace_back(up15);
        upnodes.emplace_back(up16);
        upnodes.emplace_back(up17);
        upnodes.emplace_back(up18);
        upnodes.emplace_back(up19);
        
        upnodes.emplace_back(up20);
        /*
        upnodes.emplace_back(up21);
        upnodes.emplace_back(up22);
        upnodes.emplace_back(up23);
        upnodes.emplace_back(up24);
        upnodes.emplace_back(up25);
        upnodes.emplace_back(up26);
        upnodes.emplace_back(up27);
        upnodes.emplace_back(up28);
        upnodes.emplace_back(up29);
        upnodes.emplace_back(up30);
        
        upnodes.emplace_back(up31);
        upnodes.emplace_back(up32);
        upnodes.emplace_back(up33);
        upnodes.emplace_back(up34);
        upnodes.emplace_back(up35);
        upnodes.emplace_back(up36);
        upnodes.emplace_back(up37);
        upnodes.emplace_back(up38);
        upnodes.emplace_back(up39);
        upnodes.emplace_back(up40);
        upnodes.emplace_back(up41);
        upnodes.emplace_back(up42);
        upnodes.emplace_back(up43);
        upnodes.emplace_back(up44);
        upnodes.emplace_back(up45);
        upnodes.emplace_back(up46);
        upnodes.emplace_back(up47);
        upnodes.emplace_back(up48);
        upnodes.emplace_back(up49);
        upnodes.emplace_back(up50);
        upnodes.emplace_back(up51);
        upnodes.emplace_back(up52);
        upnodes.emplace_back(up53);
        upnodes.emplace_back(up54);
        upnodes.emplace_back(up55);
        upnodes.emplace_back(up56);
        upnodes.emplace_back(up57);
        upnodes.emplace_back(up58);
        upnodes.emplace_back(up59);
        upnodes.emplace_back(up60);
        upnodes.emplace_back(up61);
        upnodes.emplace_back(up62);
        upnodes.emplace_back(up63);
        upnodes.emplace_back(up64);
        upnodes.emplace_back(up65);
        upnodes.emplace_back(up66);
        upnodes.emplace_back(up67);
        upnodes.emplace_back(up68);
        upnodes.emplace_back(up69);
        upnodes.emplace_back(up70);
        upnodes.emplace_back(up71);
        upnodes.emplace_back(up72);
        upnodes.emplace_back(up73);
        upnodes.emplace_back(up74);
        upnodes.emplace_back(up75);
        upnodes.emplace_back(up76);
        upnodes.emplace_back(up77);
        upnodes.emplace_back(up78);
        upnodes.emplace_back(up79);
        upnodes.emplace_back(up80);
        upnodes.emplace_back(up81);
        upnodes.emplace_back(up82);
        upnodes.emplace_back(up83);
        upnodes.emplace_back(up84);
        upnodes.emplace_back(up85);
        upnodes.emplace_back(up86);
        upnodes.emplace_back(up87);
        upnodes.emplace_back(up88);
        upnodes.emplace_back(up89);
        upnodes.emplace_back(up90);
        upnodes.emplace_back(up91);
        upnodes.emplace_back(up92);
        upnodes.emplace_back(up93);
        upnodes.emplace_back(up94);
        upnodes.emplace_back(up95);
        upnodes.emplace_back(up96);
        upnodes.emplace_back(up97);
        upnodes.emplace_back(up98);
        upnodes.emplace_back(up99);
        upnodes.emplace_back(up100);
*/
    }
    std::string IP="10.0.0.1";
    uint16_t port=8888;
    std::string selfpeerID="00010203040506070809101112131415161718FF"; //20 HEX number, 40 chars in total
    std::vector<UpNodeConfig> upnodes;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DownNodeConfig,IP,port,selfpeerID,upnodes)

