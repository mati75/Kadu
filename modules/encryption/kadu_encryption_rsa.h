/*
  File name: kadu_encryption_rsa.h
  Copyright: (C) 2007, 2008-2009 Tomasz Kazmierczak

  Creation date: 2007-11-08
  Last modification date: 2009-02-16

  This file is part of Kadu encryption module

 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *
 */

#ifndef KADU_ENCRYPTION_RSA_H
#define KADU_ENCRYPTION_RSA_H

#include <QtCrypto>
#include "kadu_encryption_interface.h"

/**
 * Encryption class that implements the RSA algorithm.
**/
class KaduEncryptionRSA : public KaduEncryptionInterface
{
	private:
		QCA::Initializer init;
		QString KeysPath;
		KaduEncrytpionError Error;

		bool readPubKey(QCA::PublicKey &key, QString &keyId);
		bool readPrivKey(QCA::PrivateKey &key);

	public:
		/**
		 * The only constructor. Initializes the path to the directory containing
		 * the RSA keys.
		 * 
		 * @param keysPath path to the directory containing the RSA keys
		**/
		KaduEncryptionRSA(QString keysPath) : KeysPath(keysPath), Error(KEE_NO_ERROR) {}

		/**
		 * This function generates the pair of private and public RSA keys.
		 * 
		 * @param keyId identifier for the generated keys
		 * @return true if keys were generated successfuly, false otherwise.
		 */
		bool generateKeys(QString keyId);

		/**
		 * Encrypts the given message.
		 * 
		 * @param message the message to encrypt
		 * @param keyId an identifier of a public key to use for encryption
		 * @return true if the encryption succeeded, false otherwise.
		 */
		bool encrypt(QByteArray &message, QString keyId);
		/**
		 * Decrypts the given message.
		 * 
		 * @param message the message to decrypt
		 * @return true if the decryption succeeded, false otherwise.
		 */
		bool decrypt(QByteArray &message);

		/**
		 * Returns a code of the last error.
		 * 
		 * @return The error code of the last error.
		 */
		KaduEncrytpionError error() { return Error; }
		/**
		 * Returns a description of the last error.
		 * 
		 * @return A pointer to a C-string describing the last error.
		 */
		const char *errorDescription();

		/**
		 * Releases the encryption object.
		 */
		void release();
};

#endif //KADU_ENCRYPTION_RSA_H
