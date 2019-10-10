#include"polman.hpp" //EOSIO 및 Cpp 기본 라이브러리 포함


public:
  using contract::contract;
  
  polman(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {
  }

  [[eosio::action]]
  void attachdevice(name dvice,std::string iaddr,std::string port) { //장치 추가를 위한 액션
    //require_auth(dvice); // 인증 요구
    atched_index plist(_code, _code.value);
    auto iterator = plist.find(dvice.value);
    if( iterator == plist.end() )
    {
      plist.emplace(dvice, [&]( auto& row ) {
       row.device = dvice;
      });
    };
  }
  
  
  [[eosio::action]]
  void adduser(name contractcreator,name wantuser) { //사용자 추가를 위한 액션
    if(checkdeployer(contractcreator) == 0) return; 
    puser_index ulist(_code, _code.value);
    auto iterator = ulist.find(wantuser.value);
    if( iterator == ulist.end() )
    {
      ulist.emplace(wantuser, [&]( auto& row ) {
        row.user = wantuser;
      });
    }
  }

  [[eosio::action]]
  void removedevice(name dvice) { // 장치 제거를 위한 액션
    require_auth(dvice);

    atched_index plist(_self, _code.value);
    auto iterator = plist.find(dvice.value);
    eosio_assert(iterator != plist.end(), "Record does not exist");
    plist.erase(iterator);
  }
  
  [[eosio::action]]
  void removeuser(name user) { // 사용자 제거를 위한 액션
    require_auth(user);

    puser_index ulist(_self, _code.value);
    auto iterator = ulist.find(user.value);
    eosio_assert(iterator != ulist.end(), "Record does not exist");
    ulist.erase(iterator);
  }
  
  [[eosio::action]]
  void rqstdata(name addnet,name rqster,name targetdevice){ // 자료를 읽어오기 위한 액션
    if(checkdeployer(addnet) == 0) return;
    require_auth(rqster);
    rqsted_index rqlist(_self,_code.value);
    puser_index pulist(_code, _code.value);
    auto iterator = pulist.find(rqster.value);
    auto iter = rqlist.find(rqster.value);
    if(iter == rqlist.end())
    {
      rqlist.emplace(addnet, [&]( auto& row ){
        row.rqster = rqster;
        row.device = targetdevice;
        row.payload = "";
        row.isright = !(iterator == pulist.end());
      });
    }
    else{
      rqlist.modify(iter, addnet, [&]( auto& row ){
        row.rqster = rqster;
        row.device = targetdevice;
        row.payload = "";
        row.isright = !(iterator == pulist.end());
      });
    }
  }
  
  [[eosio::action]]
  void pushdata(name addnet,name rqster,name targetdevice,std::string data){ // 
    if(checkdeployer(addnet) == 0) return;
    //require_auth(rqster);
    rqsted_index rqlist(_self,_code.value);
    puser_index pulist(_code, _code.value);
    auto iterator = pulist.find(rqster.value);
    auto iter = rqlist.find(rqster.value);
    if(iter == rqlist.end())
    {
      rqlist.emplace(addnet, [&]( auto& row ){
        row.rqster = rqster;
        row.device = targetdevice;
        row.payload = data;
        row.isright = !(iterator == pulist.end());
      });
    }
    else{
      rqlist.modify(iter, addnet, [&]( auto& row ){
        row.rqster = rqster;
        row.device = targetdevice;
        row.payload = data;
        row.isright = !(iterator == pulist.end());
      });
    }
  }
private: // 컨트랙트 스스로만 접근할수 있는 액션 및 구조체
	int checkdeployer(name tryer){
    name asd=eosio::contract::_self;
    if(tryer == asd) return 1;
    else return 0;
	  
	}
  struct [[eosio::table]] permitteduser {
    name user;
    uint64_t primary_key() const { return user.value; }
  };
  typedef eosio::multi_index<"permitlist"_n, permitteduser> puser_index;
  
  struct [[eosio::table]] permittedlist {
    name device;
    std::string iaddr;
    std::string port;
    uint64_t primary_key() const { return device.value; }
  };
  typedef eosio::multi_index<"attachedlist"_n, permittedlist> atched_index;
  
  struct [[eosio::table]] requestedlist {
	name device;
	name rqster;
    std::string payload;
    bool isright;
    uint64_t primary_key() const { return rqster.value; }
  };
  typedef eosio::multi_index<"rqstedlist"_n, requestedlist> rqsted_index;
  
  void send_rquest(name targetdevice) {
    action(
      permission_level{get_self(),"active"_n},
      get_self(),
      "notify"_n,
      std::make_tuple(targetdevice, name{targetdevice}.to_string())
    ).send();
  };
  
  
};

EOSIO_DISPATCH( polman, (attachdevice)(adduser)(removedevice)(removeuser)(rqstdata)(pushdata)) //EOSIO에서 ABI로 컴파일하기 위한 메소드 목록
