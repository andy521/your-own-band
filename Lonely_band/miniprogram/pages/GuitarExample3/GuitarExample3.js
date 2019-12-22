const id = "577848525"
const api_key = "Bc1rkda2O1j6Ji=a4ltI7XHhEmQ="
Page({
  control: function (e) {
    if (e.detail.value == true) {
      wx.request({
        url: "http://api.heclouds.com/cmds?device_id=" + id,
        method: 'POST',
        header: {
          //'content-type': 'application/x-www-form-urlencoded',
          "api-key": api_key
        },
        data: 'value2:1',

        success(res) {
          console.log(res)
          console.log("控制成功,歌曲开始演奏")
        }
      }),
        wx.showToast({
          title: '歌曲开始演奏',
          icon: 'success'
        })
    }
    else {
      wx.request({
        url: "http://api.heclouds.com/cmds?device_id=" + id,
        method: 'POST',
        header: {
          'content-type': 'application/x-www-form-urlencoded',
          "api-key": api_key
        },
        data: 'value2:2',

        success(res) {
          console.log(res)
          console.log("控制成功，歌曲停止演奏")
        }
      }),
        wx.showToast({
          title: '歌曲停止演奏',
          icon: 'none'
        })
    }
  },
  data: {
    deviceConnected: ''
  }
})
function getDeviceInfo() {
  var deviceConnected
  wx.request({
    url: "http://api.heclouds.com/devices/" + id,
    method: 'GET',
    header: {
      //'content-type': 'application/x-www-form-urlencoded',
      "api-key": api_key
    },
    data: {

    },
    success(res) {
      if (res.data.data.online!=false) {
        console.log("设备已连接")
        deviceConnected = 1
      }
      else {
        
        deviceConnected = 0
      }
    },
    fail(res) {
      console.log("请求失败")
    }
  })
}
var timer = setInterval(function () {
  getDeviceInfo()
}, 5000)
