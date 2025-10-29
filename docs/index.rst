..
   # *******************************************************************************
   # Copyright (c) 2024 Contributors to the Eclipse Foundation
   #
   # See the NOTICE file(s) distributed with this work for additional
   # information regarding copyright ownership.
   #
   # This program and the accompanying materials are made available under the
   # terms of the Apache License Version 2.0 which is available at
   # https://www.apache.org/licenses/LICENSE-2.0
   #
   # SPDX-License-Identifier: Apache-2.0
   # *******************************************************************************

.. _some_ip_gateway_architecture:

SOME/IP Gateway Architecture (Prototype)
########################################

.. toctree::
   :titlesonly:

Overview
========
The prototype imlements the basic structure as defined in the corresponding `feature request`_.
It acts as a regular mw::com ipc client on the one side and forwards as a SOME/IP client on the other side.
Actual payload transformation is not yet realized, instead binary data (interpreted as ascii text) is used for demonstration.
Although there is no actual payload transformation, the structure already foresees the "transformation plugin architecture" as defined in the  `feature request`_.
The two required plugin interfaces are realized with the SOCom infrasstructure.

**TBD:** Some 2-3 sentences to advertise SOCom.

The SOME/IP plugin exists in 2 implementations, one implementation is a test mock which simply sends the binary ascii data "Hello Gateway"
and the other implemention is an actual implementation with an AUTOSAR compatible SOME/IP stack. This implementation can just be demonstrated
but is not part of the source repository.

.. _feature request: https://eclipse-score.github.io/score/main/features/communication/some_ip_gateway/index.html


.. figure:: some_ip_gateway_prototype.drawio.svg
   :align: center
   :name: _some_ip_gateway_prototype

   General overview of the prototype of the SOME/IP Gateway

SOCom Details
=============
**TBD:** Description

.. figure:: some_ip_gateway_prototype_socom_details.drawio.svg
   :align: center
   :name: _some_ip_gateway_prototype_socom_details

   SOCom utilization within SOME/IP Gateway prototype
