<?php
// $Id: deploy_uuid_service.inc,v 1.1.2.1 2009/03/19 04:29:50 heyrocker Exp $

function deploy_uuid_service_access() {
  return TRUE;
}

function deploy_uuid_service_get_key($uuid, $module){
  return deploy_uuid_get_key($uuid, $module);
}