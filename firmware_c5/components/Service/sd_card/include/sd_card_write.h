// Copyright (c) 2025 HIGH CODE LLC
//
// TentacleOS is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TentacleOS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TentacleOS. If not, see <https://www.gnu.org/licenses/>.

#ifndef SD_CARD_WRITE_H
#define SD_CARD_WRITE_H

#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Escreve string em arquivo (sobrescreve)
 * @param path Caminho do arquivo
 * @param data String a escrever
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_string(const char *path, const char *data);

/**
 * @brief Anexa string ao arquivo
 * @param path Caminho do arquivo
 * @param data String a anexar
 * @return ESP_OK em sucesso
 */
esp_err_t sd_append_string(const char *path, const char *data);

/**
 * @brief Escreve dados binários
 * @param path Caminho do arquivo
 * @param data Buffer de dados
 * @param size Tamanho em bytes
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_binary(const char *path, const void *data, size_t size);

/**
 * @brief Anexa dados binários
 * @param path Caminho do arquivo
 * @param data Buffer de dados
 * @param size Tamanho em bytes
 * @return ESP_OK em sucesso
 */
esp_err_t sd_append_binary(const char *path, const void *data, size_t size);

/**
 * @brief Escreve uma linha (adiciona \n)
 * @param path Caminho do arquivo
 * @param line Linha a escrever
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_line(const char *path, const char *line);

/**
 * @brief Anexa uma linha (adiciona \n)
 * @param path Caminho do arquivo
 * @param line Linha a anexar
 * @return ESP_OK em sucesso
 */
esp_err_t sd_append_line(const char *path, const char *line);

/**
 * @brief Escreve dados formatados (como fprintf)
 * @param path Caminho do arquivo
 * @param format String de formato
 * @param ... Argumentos
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_formatted(const char *path, const char *format, ...);

/**
 * @brief Anexa dados formatados
 * @param path Caminho do arquivo
 * @param format String de formato
 * @param ... Argumentos
 * @return ESP_OK em sucesso
 */
esp_err_t sd_append_formatted(const char *path, const char *format, ...);

/**
 * @brief Escreve buffer em arquivo
 * @param path Caminho do arquivo
 * @param buffer Buffer de dados
 * @param size Tamanho do buffer
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_buffer(const char *path, const void *buffer, size_t size);

/**
 * @brief Escreve array de bytes
 * @param path Caminho do arquivo
 * @param bytes Array de bytes
 * @param count Número de bytes
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_bytes(const char *path, const uint8_t *bytes, size_t count);

/**
 * @brief Escreve um único byte
 * @param path Caminho do arquivo
 * @param byte Byte a escrever
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_byte(const char *path, uint8_t byte);

/**
 * @brief Escreve inteiro em arquivo
 * @param path Caminho do arquivo
 * @param value Valor inteiro
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_int(const char *path, int32_t value);

/**
 * @brief Escreve float em arquivo
 * @param path Caminho do arquivo
 * @param value Valor float
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_float(const char *path, float value);

/**
 * @brief Escreve CSV com múltiplas colunas
 * @param path Caminho do arquivo
 * @param columns Array de strings (colunas)
 * @param num_columns Número de colunas
 * @return ESP_OK em sucesso
 */
esp_err_t sd_write_csv_row(const char *path, const char **columns, size_t num_columns);

/**
 * @brief Anexa linha CSV
 * @param path Caminho do arquivo
 * @param columns Array de strings
 * @param num_columns Número de colunas
 * @return ESP_OK em sucesso
 */
esp_err_t sd_append_csv_row(const char *path, const char **columns, size_t num_columns);

#ifdef __cplusplus
}
#endif

#endif /* SD_CARD_WRITE_H */