
CREATE TABLE `tb_data_pv_20180125` (
  `time` datetime DEFAULT NULL COMMENT '时间',

  `id` varchar(64) DEFAULT NULL COMMENT '落地页id',
  `ty` int(11) DEFAULT NULL COMMENT '请求的类型:1,访问 2,跳出 3,二跳',
  
  `bid` varchar(64) DEFAULT NULL COMMENT 'bid',
  `mid` varchar(64) DEFAULT NULL COMMENT '创意id',
  `datasrc` varchar(32) DEFAULT NULL COMMENT '数据来源平台:dsp/pap',
  
  `os` int(11) DEFAULT NULL COMMENT '操作系统',
  `ds` varchar(64) DEFAULT NULL COMMENT '移动设备分别率',
  
  `tp` int(11) DEFAULT NULL COMMENT '移动类型',
  `op` int(11) DEFAULT NULL COMMENT '运营商',
  `nw` int(11) DEFAULT NULL COMMENT '联网类型',
  `mb` int(11) DEFAULT NULL COMMENT '设备型号',
  
  `pu` varchar(1024) DEFAULT NULL COMMENT '上游连接',
  `cl` varchar(1024) DEFAULT NULL COMMENT '受访页面',
  
  `pv` int(11) DEFAULT NULL COMMENT '展现数',
  `uv` int(11) DEFAULT NULL COMMENT '用户数',
  
  `hf` varchar(1024) DEFAULT NULL COMMENT '二跳的URL',
  `at` int(11) DEFAULT NULL COMMENT '平均访问时长',
  
  `ip` varchar(32) DEFAULT NULL COMMENT 'ip',
  `region` int(11) DEFAULT NULL COMMENT '地域编码'

) ENGINE=MyISAM DEFAULT CHARSET=utf8;

