import logging
import re

import boto3

from cpputils import default_ca_path, ini_load

__config_keys_cfg_file = [
    "s3_config_file",
    "aws_config_file",
    "aws_shared_credentials_file",
]

__default_config_values_cfg_file = {
    "ssl_cafile": default_ca_path(),
    "fetch_max_bytes": 20000000,
}

__config_keys_client = {
    "signurl_use_https": ["signurl_use_https"],
    "aws_access_key_id": ["access_key"],
    "aws_secret_access_key": ["secret_key"],
    "endpoint_url": ["endpoint-url", "host_base"],
    "region_name": ["bucket_location", "region", "aws_default_region"],
    "service_name": ["service_name"],
    "verify": ["ca_certs", "aws_ca_bundle", "ca_bundle"],
}

__default_config_values_client = {
    "region_name": "us-east-1",
    "signurl_use_https": True,
    "verify": default_ca_path(),
    "service_name": "s3",
}


def boto3_client_args(cfg_file=None, section=None, **kwargs):
    """
    Function to read aws, s3cfg files or environment variables and provide for boto3 client

    :param str cfg_file:
    :param str section:
    :param **kwargs boto3.client args

    See :py:meth:`boto3.session.Session.client`.
    """
    try:
        if not cfg_file:
            logging.warning(
                "No cfg_file set or found. Using default AWS Config/Credentials or Env-Variables."
            )
            cfg_file = ini_load(
                keys=__config_keys_cfg_file,
                defaults=__default_config_values_cfg_file,
            )
        if not section:
            section = ini_load(
                keys="aws_profile",
                defaults="default",
            )["aws_profile"]

        logging.info(f"read files {cfg_file} and section {section}")

        __client_args = ini_load(
            files=cfg_file,
            sections=section,
            keys=__config_keys_client,
            defaults=__default_config_values_client,
        )

        if (
            "endpoint_url" in __client_args
            and __client_args["endpoint_url"] is not None
            and not re.match(__client_args["endpoint_url"], "http")
        ):
            __client_args["endpoint_url"] = (
                "https://"
                if "signurl_use_https" in __client_args
                and __client_args["signurl_use_https"]
                else "http://"
            ) + __client_args["endpoint_url"]

        logging.info(
            "Using s3 client args: ["
            + (
                str(__client_args["endpoint_url"])
                if "endpoint_url" in __client_args
                else "no endpoint"
            )
            + ", "
            + (
                str(__client_args["region_name"])
                if "region_name" in __client_args
                else "no region"
            )
            + ", has access_key: "
            + (
                str(__client_args["aws_access_key_id"] not in ["", None])
                if "aws_access_key_id" in __client_args
                else "False"
            )
            + ", has secret_key: "
            + (
                str(__client_args["aws_secret_access_key"] not in ["", None])
                if "aws_access_key_id" in __client_args
                else "False"
            )
        )

        __client_args.update(kwargs)
        return {
            k: v
            for k, v in __client_args.items()
            if k in boto3.session.Session.client.__code__.co_varnames
        }

    except Exception as error:
        raise error from error
