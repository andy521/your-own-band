Page({
  getMyInfo:function(e){
    let info=e.detail.userInfo;
    this.setData({
      isLogin:true,
      src:info.avatarUrl,
      nickName:info.nickName
    })
  }
})